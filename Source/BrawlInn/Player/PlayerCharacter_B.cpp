// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter_B.h"
#include "BrawlInn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Camera/CameraComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"

#include "System/Interfaces/ControllerInterface_B.h"
#include "Player/PlayerController_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "Components/FireDamageComponent_B.h"
#include "Components/WidgetComponent.h"
#include "Components/HealthComponent_B.h"
#include "System/DamageTypes/Fire_DamageType_B.h"
#include "System/DamageTypes/Barrel_DamageType_B.h"
#include "Items/Throwable_B.h"
#include "System/GameMode_B.h"
#include "System/Camera/GameCamera_B.h"
#include "Animations/PlayerAnimInstance_B.h"

APlayerCharacter_B::APlayerCharacter_B()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;

	HoldComponent = CreateDefaultSubobject<UHoldComponent_B>("Hold Component");
	HoldComponent->SetupAttachment(GetMesh());
	ThrowComponent = CreateDefaultSubobject<UThrowComponent_B>("Throw Component");
	FireDamageComponent = CreateDefaultSubobject<UFireDamageComponent_B>("Fire Damage Component");

	PunchComponent = CreateDefaultSubobject<UPunchComponent_B>("PunchComponent");
	PunchComponent->SetupAttachment(GetMesh(), "PunchCollisionHere");
	PunchComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HealthWidget = CreateDefaultSubobject<UWidgetComponent>("Widget Component");
	
	DirectionIndicatorPlane = CreateDefaultSubobject<UStaticMeshComponent>("Direction Indicator Plane");
	DirectionIndicatorPlane->SetupAttachment(RootComponent);
	DirectionIndicatorPlane->SetRelativeLocation(FVector(20, 0, -70));
	DirectionIndicatorPlane->SetRelativeRotation(FRotator(0, 90, 0));
	DirectionIndicatorPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DirectionIndicatorPlane->SetRelativeScale3D(FVector(3.327123, 3.327123, 1));

	GetCharacterMovement()->MaxWalkSpeed = 2000; 
	GetCharacterMovement()->MaxAcceleration = 800;
	GetCharacterMovement()->BrakingFrictionFactor = 1;
	GetCharacterMovement()->GroundFriction = 3;
}

void APlayerCharacter_B::BeginPlay()
{
	Super::BeginPlay();
	NormalMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	//caches mesh transform to reset it every time player gets up.
	RelativeMeshTransform = GetMesh()->GetRelativeTransform();
	OnTakeRadialDamage.AddDynamic(this, &APlayerCharacter_B::OnRadialDamageTaken);
	FireDamageComponent->FireHealthIsZero_D.AddDynamic(this, &APlayerCharacter_B::TakeFireDamage);

	MakeInvulnerable(1.0f);

	for (TActorIterator<AGameCamera_B> itr(GetWorld()) ; itr; ++itr)
	{
		GameCamera = *itr;	
	}
}

void APlayerCharacter_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetState() == EState::EFallen)
	{
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), FindMeshLocation(), DeltaTime, 50));
	}
	else if (GetState() == EState::EBeingHeld)
	{
		CurrentHoldTime += DeltaTime;
		if (CurrentHoldTime >= MaxHoldTime)
			BreakFree();
	}
	else
	{
		CheckFall(DeltaTime);
		if (!(GetState() == EState::EStunned))
		{
			HandleMovement(DeltaTime);
			HandleRotation(DeltaTime);
		}
	}

	UpdateHealthRotation();
}

void APlayerCharacter_B::TakeFireDamage()
{
	Fall(PunchedRecoveryTime);
	MakeInvulnerable(InvulnerabilityTime + PunchedRecoveryTime);
	IControllerInterface_B* Interface = Cast<IControllerInterface_B>(GetController());
	if (Interface)
	{
		Interface->Execute_TakeOneDamage(GetController());
	}
}

float APlayerCharacter_B::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsInvulnerable) return 0;
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UFire_DamageType_B::StaticClass()))
	{
		FireDamageComponent->FireDamageStart_D.Broadcast();
	}
	else if (DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UBarrel_DamageType_B::StaticClass()))
	{
		ApplyDamageMomentum(DamageAmount, DamageEvent, nullptr, DamageCauser);
	}
	else
	{
		IControllerInterface_B* Interface = Cast<IControllerInterface_B>(GetController());
		if (Interface)
		{
			Interface->Execute_TakeOneDamage(GetController());
		}
	}
	return DamageAmount;
}

void APlayerCharacter_B::UpdateHealthRotation()
{
	if (!GameCamera)
		return;

	HealthWidget->SetWorldRotation((GameCamera->Camera->GetForwardVector() * -1).Rotation());
	
	FVector2D DrawSize;
	
	float xMin = 25.f;
	float xMax = 100.f;

	float minPosZ = 150;
	float maxPosZ = 200;
	
	float size = (GameCamera->SpringArm->TargetArmLength - GameCamera->SmallestSpringArmLength) *
		((xMax - xMin) / (GameCamera->LargestSpringArmLength - GameCamera->SmallestSpringArmLength)) + xMin;

	float posZ = (GameCamera->SpringArm->TargetArmLength - GameCamera->SmallestSpringArmLength) *
		((maxPosZ - minPosZ) / (GameCamera->LargestSpringArmLength - GameCamera->SmallestSpringArmLength)) + minPosZ;

	DrawSize.X = size * 4;
	DrawSize.Y = size * 4;

	HealthWidget->RelativeLocation.Z = posZ;
	HealthWidget->SetDrawSize(DrawSize);
}

void APlayerCharacter_B::HandleRotation(float DeltaTime)
{
	if (InputVector.Size() > 0.1)
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), InputVector.ToOrientationRotator(), DeltaTime, 10.f));
}

void APlayerCharacter_B::HandleMovement(float DeltaTime)
{
	//Normalizes to make sure we dont accelerate faster diagonally, but still want to allow for slower movement.
	if (InputVector.SizeSquared() >= 1.f)
		InputVector.Normalize();
	GetMovementComponent()->AddInputVector(InputVector);
}

void APlayerCharacter_B::CheckFall(float DeltaTime)
{
	float Speed = GetMovementComponent()->Velocity.Size();
	if (Speed >= NormalMaxWalkSpeed * FallLimitMultiplier)
	{
		Fall(PunchedRecoveryTime);
		MakeInvulnerable(InvulnerabilityTime + PunchedRecoveryTime);
	}
}

void APlayerCharacter_B::Fall(float RecoveryTime)
{
	if (!GetCharacterMovement()->IsFalling())
	{
		if (HoldComponent && HoldComponent->IsHolding())
		{
			HoldComponent->Drop();
		}
		SetState(EState::EFallen);
		GetMesh()->SetGenerateOverlapEvents(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AddImpulse(GetMovementComponent()->Velocity, "ProtoPlayer_BIND_SpineTop_JNT_center", true);

		if (PlayerController)
			PlayerController->PlayControllerVibration(1.f, 0.5f, true, true, true, true);

		GetWorld()->GetTimerManager().SetTimer(TH_RecoverTimer, this, &APlayerCharacter_B::StandUp, RecoveryTime, false);
	}
	else
		BWarn("Character is in air! Fall() does not run!");
}

FVector APlayerCharacter_B::FindMeshLocation()
{
	//find a specified socket (bone)
	FVector MeshLoc = GetMesh()->GetSocketLocation("pelvis");
	//ray trace to ground
	FHitResult Hit;
	bool bDidHit = GetWorld()->LineTraceSingleByChannel(Hit, MeshLoc + FVector(0, 0, 0), MeshLoc + FVector(0, 0, -1000), ECollisionChannel::ECC_Visibility);

	if (bDidHit)
		return (Hit.Location - RelativeMeshTransform.GetLocation());
	else
		return (MeshLoc - RelativeMeshTransform.GetLocation());
}

void APlayerCharacter_B::StandUp()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		//Saves snapshot for blending to animation
		GetMesh()->GetAnimInstance()->SavePoseSnapshot("Ragdoll");

		SetState(EState::EWalking);
		GetMovementComponent()->StopMovementImmediately();

		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetMesh()->SetGenerateOverlapEvents(false);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

//a number less than 0 will make the character invulnerable until toggled off again
void APlayerCharacter_B::MakeInvulnerable(float ITime)
{
	bIsInvulnerable = true;

	if (InvulnerableMat)
		GetMesh()->SetMaterial(6, InvulnerableMat);

	if (ITime > 0)
		GetWorld()->GetTimerManager().SetTimer(TH_InvincibilityTimer, this, &APlayerCharacter_B::MakeVulnerable, ITime, false);
}

void APlayerCharacter_B::MakeVulnerable()
{
	bIsInvulnerable = false;

	if (InvulnerableMat)
		GetMesh()->SetMaterial(6, InvisibleMat);
}

bool APlayerCharacter_B::IsInvulnerable()
{
	return bIsInvulnerable;
}

void APlayerCharacter_B::AddStun()
{
	if (!(GetState() == EState::EStunned))
	{
		StunAmount++;
		if (StunAmount >= PunchesToStun)
		{
			SetState(EState::EStunned);
		}
		GetWorld()->GetTimerManager().SetTimer(TH_StunTimer, this, &APlayerCharacter_B::RemoveStun, StunTime, false);
	}
}

void APlayerCharacter_B::RemoveStun()
{
	if (GetState() == EState::EStunned)
	{
		SetState(EState::EWalking);
	}
	StunAmount = 0;
}

void APlayerCharacter_B::PickedUp_Implementation(APlayerCharacter_B* Player)
{
	HoldingPlayer = Player;
	GetMovementComponent()->StopMovementImmediately();
	SetState(EState::EBeingHeld);
	GetWorld()->GetTimerManager().ClearTimer(TH_RecoverTimer);
	GetMesh()->SetSimulatePhysics(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetGenerateOverlapEvents(false);

	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->StopMovementImmediately();
	SetActorRotation(FRotator(-90, 0, 90));
}

void APlayerCharacter_B::Dropped_Implementation()
{
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DetachFromActor(rules);
	Fall(PunchedRecoveryTime);
	GetMesh()->SetSimulatePhysics(true);
	HoldingPlayer = nullptr;
	SetActorRotation(FRotator(0, 0, 0));
	CurrentHoldTime = 0.f;
}

void APlayerCharacter_B::Use_Implementation()
{
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DetachFromActor(rules);
	Fall(PunchedRecoveryTime);
	GetMesh()->SetSimulatePhysics(true);

	/// Throw with the help of AimAssist.
	FVector TargetLocation = HoldingPlayer->GetActorForwardVector();
	HoldingPlayer->ThrowComponent->AimAssist(TargetLocation);
	GetMesh()->AddImpulse(TargetLocation * HoldingPlayer->ThrowComponent->ImpulseSpeed, "ProtoPlayer_BIND_SpineTop_JNT_center", true);

	HoldingPlayer = nullptr;

	SetActorRotation(FRotator(0, 0, 0));
}

void APlayerCharacter_B::SetState(EState s)
{
	State = s;
	switch (State) 
	{
	case EState::EWalking:
		GetCharacterMovement()->MaxWalkSpeed = NormalMaxWalkSpeed;
		break;
	case EState::EHolding:
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
		break;
	}
}

EState APlayerCharacter_B::GetState() const
{
	return State;
}

bool APlayerCharacter_B::IsHeld_Implementation() const
{
	if (HoldingPlayer)
		return true;
	return false;
}

APlayerController_B* APlayerCharacter_B::GetPlayerController_B() const
{
	return PlayerController;
}

void APlayerCharacter_B::FellOutOfWorld(const UDamageType& dmgType)
{
	UGameplayStatics::ApplyDamage(this, FellOutOfWorldDamageAmount, PlayerController, this, dmgType.StaticClass());
	if (PlayerController)
	{
		AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->DespawnCharacter_D.Broadcast(PlayerController);
			GameMode->SpawnCharacter_D.Broadcast(PlayerController);
		}
		else
		{
			BError("GameMode Could Not Be Found!");
		}
	}
	else
	{
		Destroy();
	}
}

void APlayerCharacter_B::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PlayerController = Cast<APlayerController_B>(NewController);

}

void APlayerCharacter_B::PunchButtonPressed()
{
	if (!(GetState() == EState::EWalking)) { return; }
	if (PunchComponent)
	{
		PunchComponent->bIsPunching = true;
	}
	else
	{
		BError("No Punch Component for player %s", *GetNameSafe(this));
	}
}

void APlayerCharacter_B::BreakFreeButtonMash()
{
	CurrentHoldTime += 0.01;
}

void APlayerCharacter_B::BreakFree()
{
	//Detaches usselves from the other player
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DetachFromActor(rules);
	SetActorRotation(FRotator(0, 0, 0));

	//Moves forward a bit before turning on collisions, so we dont fly6 to hell because we overlap with the other player
	if (HoldingPlayer)
		AddActorLocalOffset(HoldingPlayer->GetActorForwardVector() * 100);
	StandUp();

	CurrentHoldTime = 0.f;
	//Fixes up the other player
	if (HoldingPlayer)
	{
		HoldingPlayer->HoldComponent->SetHoldingItem(nullptr);
		HoldingPlayer->AddStun();
		HoldingPlayer = nullptr;
	}
}

void APlayerCharacter_B::OnRadialDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	BWarn("Radial Damage Taken!");
	if (!bIsInvulnerable)
	{
		//Calculates force vector
		FVector Direction = GetActorLocation() - Origin;
		Direction.Normalize();
		Direction *= DamageType->DamageImpulse;

		GetCharacterMovement()->AddImpulse(Direction);
	}
}
