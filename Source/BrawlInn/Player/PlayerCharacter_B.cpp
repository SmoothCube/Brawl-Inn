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
#include "Components/WidgetComponent.h"
#include "Components/HealthComponent_B.h"
#include "System/DamageTypes/Fire_DamageType_B.h"
#include "System/DamageTypes/Barrel_DamageType_B.h"
#include "Items/Throwable_B.h"
#include "System/GameMode_B.h"
#include "System/Camera/GameCamera_B.h"

APlayerCharacter_B::APlayerCharacter_B()
{
	bUseControllerRotationYaw = false;

	HealthWidget = CreateDefaultSubobject<UWidgetComponent>("HealthWidgetComponent");

	DirectionIndicatorPlane = CreateDefaultSubobject<UStaticMeshComponent>("Direction Indicator Plane");
	DirectionIndicatorPlane->SetupAttachment(RootComponent);
	DirectionIndicatorPlane->SetRelativeLocation(FVector(20, 0, -70));
	DirectionIndicatorPlane->SetRelativeRotation(FRotator(0, 90, 0));
	DirectionIndicatorPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DirectionIndicatorPlane->SetRelativeScale3D(FVector(3.327123, 3.327123, 1));
}

void APlayerCharacter_B::BeginPlay()
{
	Super::BeginPlay();

	//Should be moved into health widget
	for (TActorIterator<AGameCamera_B> itr(GetWorld()); itr; ++itr)
	{
		GameCamera = *itr;
	}

	MakeInvulnerable(1.0f);
}

void APlayerCharacter_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UpdateHealthRotation();
}

float APlayerCharacter_B::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsInvulnerable || bHasShield) return 0;
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UBarrel_DamageType_B::StaticClass()))
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

	FVector Location = HealthWidget->GetRelativeLocation();
	Location.Z = posZ;
	HealthWidget->SetRelativeLocation(Location);
	HealthWidget->SetDrawSize(DrawSize);
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

	if (InvulnerableMat && !bHasShield)
		GetMesh()->SetMaterial(6, InvisibleMat);
}

bool APlayerCharacter_B::IsInvulnerable()
{
	return bIsInvulnerable;
}

void APlayerCharacter_B::ApplyShield()
{
	bHasShield = true;

	if (ShieldMat)
		GetMesh()->SetMaterial(6, ShieldMat);
}

void APlayerCharacter_B::RemoveShield()
{
	if (!bHasShield)
		return;

	bHasShield = false;

	if (ShieldMat)
		GetMesh()->SetMaterial(6, InvisibleMat);
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

bool APlayerCharacter_B::CheckFall(float DeltaTime)
{
	bool s = Super::CheckFall(DeltaTime);
	if (s)
	{
		MakeInvulnerable(InvulnerabilityTime + PunchedRecoveryTime);
	}
	return s;
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
		BLog("Playercontroller: %s", *GetNameSafe(PlayerController));
		AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			BLog("GameMode %s", *GetNameSafe(GameMode));
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
		Super::FellOutOfWorld(dmgType);
	}
}

void APlayerCharacter_B::OnRadialDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!bIsInvulnerable)
	{
		if (bHasShield)
			RemoveShield();
		else
			Super::OnRadialDamageTaken(DamagedActor, Damage, DamageType, Origin, HitInfo, InstigatedBy, DamageCauser);
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