// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter_B.h"
#include "BrawlInn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"

#include "System/Interfaces/ControllerInterface_B.h"
#include "Player/PlayerController_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "Components/FireDamageComponent_B.h"
#include "Components/HealthComponent_B.h"
#include "System/DamageTypes/Fire_DamageType_B.h"
#include "System/DamageTypes/Barrel_DamageType_B.h"
#include "Items/Throwable_B.h"
#include "System/GameMode_B.h"
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

	GetCharacterMovement()->MaxCustomMovementSpeed = 600.f;
}

void APlayerCharacter_B::BeginPlay()
{
	Super::BeginPlay();

	//caches mesh transform to reset it every time player gets up.
	RelativeMeshTransform = GetMesh()->GetRelativeTransform();
	//GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter_B::CapsuleBeginOverlap);
	OnTakeRadialDamage.AddDynamic(this, &APlayerCharacter_B::OnRadialDamageTaken);
	FireDamageComponent->FireHealthIsZero_D.AddDynamic(this, &APlayerCharacter_B::TakeFireDamage);

}

void APlayerCharacter_B::TakeFireDamage()
{
	Fall();
	MakeInvulnerable(InvulnerabilityTime + RecoveryTime);
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

void APlayerCharacter_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (State == EState::EFallen)
	{
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), FindMeshLocation(), DeltaTime, 50));
	}
	//else if (State == EState::EStunned)
	//{

	//}
	else
	{
		if (State == EState::EWalking)
			HandleMovement(DeltaTime);
		else if (State == EState::EHolding)
			HandleMovementHold();
		HandleRotation(DeltaTime);
	}
}

void APlayerCharacter_B::HandleRotation(float DeltaTime)
{
	/*if (RotationVector.Size() > 0.1)
	{
		RotationVector.Normalize();
		SetActorRotation(RotationVector.ToOrientationRotator());
	}*/
	if (InputVector.Size() > 0.1)
	{
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), InputVector.ToOrientationRotator(), DeltaTime, 10.f));

	}
	//PrevRotation = GetActorRotation();
}

void APlayerCharacter_B::HandleMovement(float DeltaTime)
{
	//shouldnt set this each tick
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;

	//Normalizes to make sure we dont move faster than max speed, but still want to allow for slower movement.
	if (InputVector.SizeSquared() >= 1.f)
		InputVector.Normalize();
	float Speed = GetMovementComponent()->Velocity.Size();
	if (Speed >= GetMovementComponent()->GetMaxSpeed() * 0.9f)
	{
		CurrentFallTime += DeltaTime;
		float Intensity = CurrentFallTime + 0.7;
		if (!PunchComponent->bIsPunching && PlayerController)
			PlayerController->PlayControllerVibration(Intensity, 0.1f, true, true, true, true);
	
		if (Speed >= GetMovementComponent()->GetMaxSpeed() * FallLimitMultiplier)
		{
			Fall();
			MakeInvulnerable(InvulnerabilityTime + RecoveryTime);
		}
	}
	else
	{
		CurrentFallTime = 0;
	}
	GetMovementComponent()->AddInputVector(InputVector);
}

void APlayerCharacter_B::HandleMovementHold()
{
	//Shouldnt set this each tick
	GetCharacterMovement()->MaxWalkSpeed = 600.f;

	if (InputVector.SizeSquared() >= 1.f)
		InputVector.Normalize();
	GetCharacterMovement()->AddInputVector(InputVector);
}

void APlayerCharacter_B::Fall()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		if (HoldComponent && HoldComponent->IsHolding())
		{
			HoldComponent->Drop();
		}

		State = EState::EFallen;
		BWarn("Falling! Velocity: %s", *GetMovementComponent()->Velocity.ToString());
		GetMesh()->SetGenerateOverlapEvents(true);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AddImpulse(GetMovementComponent()->Velocity, "ProtoPlayer_BIND_SpineTop_JNT_center", true);

		//Decides which parts of the controller to vibrate. Overkill? probably but i wanted to test it.
		bool RU, RD, LU, LD;
		RU = RD = LU = LD = true; // false

		if (PlayerController)
			PlayerController->PlayControllerVibration(1.f, 0.5f, LU, LD, RU, RD);

		GetWorld()->GetTimerManager().SetTimer(TH_RecoverTimer, this, &APlayerCharacter_B::StandUp, RecoveryTime, false);
	}
}

FVector APlayerCharacter_B::FindMeshLocation()
{
	//find a specified socket (bone)
	FVector MeshLoc = GetMesh()->GetSocketLocation("pelvis");
	//ray trace to ground
	FHitResult Hit;
	bool bDidHit = GetWorld()->LineTraceSingleByChannel(Hit, MeshLoc + FVector(0, 0, 0), MeshLoc + FVector(0, 0, -1000), ECollisionChannel::ECC_Visibility);

	if (bDidHit)
	{
		return (Hit.Location - RelativeMeshTransform.GetLocation());
	}
	else
	{
		return (MeshLoc - RelativeMeshTransform.GetLocation());
	}
}

void APlayerCharacter_B::StandUp()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		//UE_LOG(LogTemp, Warning, TEXT("[APlayerCharacter_B::StandUp] Getting Up!"));
		GetMesh()->GetAnimInstance()->SavePoseSnapshot("Ragdoll");
		State = EState::EWalking;
		GetMovementComponent()->StopMovementImmediately();
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetMesh()->SetGenerateOverlapEvents(false);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

//a number less than 0 will make the character invincible until toggled off again
void APlayerCharacter_B::MakeInvulnerable(float InvincibilityTime)
{
	bIsInvulnerable = true;

	if(InvulnerableMat)
		GetMesh()->SetMaterial(6,InvulnerableMat);
	if(InvincibilityTime > 0)
		GetWorld()->GetTimerManager().SetTimer(TH_InvincibilityTimer, this, &APlayerCharacter_B::MakeVulnerable, InvincibilityTime, false);
}

void APlayerCharacter_B::MakeVulnerable()
{
	if (InvulnerableMat)
		GetMesh()->SetMaterial(6, InvisibleMat);
	bIsInvulnerable = false;
}

bool APlayerCharacter_B::IsInvulnerable()
{
	return bIsInvulnerable;
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
	//if (State == EState::EStunned) { return; }
	if (PunchComponent)
	{
		PunchComponent->bIsPunching = true;
	}
	else
	{
		BError("No Punch Component for player %s", *GetNameSafe(this));
	}
}

//TODO Cleanup, this is currently not in use. Adddynamic on capsule is commented out.
void APlayerCharacter_B::CapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AThrowable_B* Item(Cast<AThrowable_B>(OtherActor));
	if (Item)
	{
		//	BWarn("Overlaping with %s, Velocity %f", *GetNameSafe(Item), *Item->GetVelocity().ToString());
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
