// Fill out your copyright notice in the Description page of Project Settings.

#include "Character_B.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"
#include "Sound/SoundCue.h"
#include "NiagaraComponent.h"
#include "Animation/AnimInstance.h"

#include "BrawlInn.h"
#include "System/GameInstance_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "System/DamageTypes/Barrel_DamageType_B.h"

ACharacter_B::ACharacter_B()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;

	HoldComponent = CreateDefaultSubobject<UHoldComponent_B>("Hold Component");
	HoldComponent->SetupAttachment(GetMesh());
	ThrowComponent = CreateDefaultSubobject<UThrowComponent_B>("Throw Component");

	PunchComponent = CreateDefaultSubobject<UPunchComponent_B>("PunchComponent");
	PunchComponent->SetupAttachment(GetMesh(), "PunchCollisionHere");
	PunchComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->MaxWalkSpeed = 1800.f;
	GetCharacterMovement()->MaxAcceleration = 1500;
	GetCharacterMovement()->BrakingFrictionFactor = 1;
	GetCharacterMovement()->GroundFriction = 3;

	GetCapsuleComponent()->SetCapsuleHalfHeight(91.f);
	GetCapsuleComponent()->SetCapsuleRadius(60.f);
//	GetCapsuleComponent()->SetMassOverrideInKg(NAME_None, 161.f);
//	GetCapsuleComponent()->SetCollisionProfileName("Capsule");

	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	PS_Stun = CreateDefaultSubobject<UNiagaraComponent>("Stun Particle System");
	PS_Stun->SetupAttachment(GetMesh());
	PS_Stun->SetRelativeLocation( FVector(0.000000, -20.000000, 180.000000));

	PS_Charge = CreateDefaultSubobject<UNiagaraComponent>("Charge Particle System");
	PS_Charge->SetupAttachment(GetMesh(), "PunchCollisionHere");
}

void ACharacter_B::BeginPlay()
{
	Super::BeginPlay();

	NormalMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	//caches mesh transform to reset it every time player gets up.
	RelativeMeshTransform = GetMesh()->GetRelativeTransform();

	PS_Stun->Deactivate();
	PS_Charge->Deactivate();

	MakeInvulnerable(1.0f);
}

void ACharacter_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetState() == EState::EFallen)
	{
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), FindMeshLocation(), DeltaTime, 50));
	}
	else if (GetState() != EState::EBeingHeld)
	{
		HandleMovement(DeltaTime);
	}
}

void ACharacter_B::HandleMovement(float DeltaTime)
{
	//Normalizes to make sure we dont accelerate faster diagonally, but still want to allow for slower movement.
	if (InputVector.SizeSquared() >= 1.f)
		InputVector.Normalize();
	GetMovementComponent()->AddInputVector(InputVector);

	if (InputVector.SizeSquared() > 0)
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), InputVector.ToOrientationRotator(), DeltaTime, RotationInterpSpeed));
}

void ACharacter_B::CheckFall(FVector MeshForce)
{
	if (PunchComponent->bIsPunching || bIsInvulnerable)
		return;

	Fall(MeshForce, FallRecoveryTime);

}

void ACharacter_B::Fall(FVector MeshForce, float RecoveryTime)
{
	if (GetCharacterMovement()->IsFalling())
		return;

	if (HoldComponent && HoldComponent->IsHolding())
		HoldComponent->Drop();

	SetState(EState::EFallen);

	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->AddImpulse(MeshForce, ForceSocketName, false);	//TODO make the bone dynamic instead of a variable

	if (RecoveryTime >= 0 && bIsAlive)
		GetWorld()->GetTimerManager().SetTimer(TH_FallRecoverTimer, this, &ACharacter_B::StandUp, RecoveryTime, false);
}

void ACharacter_B::StandUp()
{
	if (GetCharacterMovement()->IsFalling())
		return;

	//Saves snapshot for blending to animation
	GetMesh()->GetAnimInstance()->SavePoseSnapshot("Ragdoll");

	GetMovementComponent()->StopMovementImmediately();

	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetGenerateOverlapEvents(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MakeInvulnerable(InvulnerabilityTime);
	SetState(EState::EWalking);
	StunAmount = 0;
}

FVector ACharacter_B::FindMeshLocation()
{
	FVector MeshLoc = GetMesh()->GetSocketLocation("pelvis");

	FHitResult Hit;
	bool bDidHit = GetWorld()->LineTraceSingleByChannel(Hit, MeshLoc + FVector(0, 0, 0), MeshLoc + FVector(0, 0, -1000), ECollisionChannel::ECC_Visibility);

	if (bDidHit)
		return (Hit.Location - RelativeMeshTransform.GetLocation());
	else
		return (MeshLoc - RelativeMeshTransform.GetLocation());
}

void ACharacter_B::PickedUp_Implementation(ACharacter_B* Player)
{
	HoldingCharacter = Player;
	GetMovementComponent()->StopMovementImmediately();
	SetState(EState::EBeingHeld);
	GetWorld()->GetTimerManager().ClearTimer(TH_FallRecoverTimer);
	GetMesh()->SetSimulatePhysics(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetGenerateOverlapEvents(false);

	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->StopMovementImmediately();
	SetActorRotation(FRotator(-90, 0, 90));
}

void ACharacter_B::Dropped_Implementation()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Fall(FVector::ZeroVector, FallRecoveryTime);
	GetMesh()->SetSimulatePhysics(true);
	HoldingCharacter = nullptr;
	SetActorRotation(FRotator(0, 0, 0));
}

void ACharacter_B::Use_Implementation()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	if (IsValid(HoldingCharacter) && IsValid(HoldingCharacter->ThrowComponent))
	{
		FVector TargetLocation = HoldingCharacter->GetActorForwardVector();
		HoldingCharacter->ThrowComponent->AimAssist(TargetLocation);
		Fall(TargetLocation * HoldingCharacter->ThrowComponent->ImpulseSpeed, FallRecoveryTime);
	}
	GetMesh()->SetSimulatePhysics(true);

	HoldingCharacter = nullptr;

	SetActorRotation(FRotator(0, 0, 0));
}

bool ACharacter_B::IsHeld_Implementation() const
{
	if (HoldingCharacter)
		return true;
	return false;
}

bool ACharacter_B::CanBeHeld_Implementation() const
{
	return bCanBeHeld;
}

void ACharacter_B::AddStun(int Strength)
{
	if (StunAmount == PunchesToStun - 1)
	{
		StunAmount += Strength;
		return;
	}
		StunAmount += Strength;
	if (StunAmount >= PunchesToStun -1)
	{
		//StunAmount = PunchesToStun - 1;
		//if (IsValid(PS_Stun))
		//	PS_Stun->Activate();
	}
}

void ACharacter_B::MakeInvulnerable(float ITime, bool bShowInvulnerabilityEffect)
{
	bIsInvulnerable = true;

	if (bShowInvulnerabilityEffect && InvulnerableMat)
		GetMesh()->SetMaterial(SpecialMaterialIndex, InvulnerableMat);
	if (ITime > 0)
		GetWorld()->GetTimerManager().SetTimer(TH_InvincibilityTimer, this, &ACharacter_B::MakeVulnerable, ITime, false);
}

void ACharacter_B::MakeVulnerable()
{
	bIsInvulnerable = false;

	if (InvulnerableMat && !bHasShield)
		GetMesh()->SetMaterial(SpecialMaterialIndex, InvisibleMat);
}

bool ACharacter_B::IsInvulnerable()
{
	return bIsInvulnerable;
}

void ACharacter_B::ApplyShield()
{
	bHasShield = true;

	if (ShieldMat)
		GetMesh()->SetMaterial(SpecialMaterialIndex, ShieldMat);
}

void ACharacter_B::RemoveShield()
{
	bHasShield = false;

	if (ShieldMat)
		GetMesh()->SetMaterial(SpecialMaterialIndex, InvisibleMat);
}

bool ACharacter_B::HasShield() const
{
	return bHasShield;
}

void ACharacter_B::SetState(EState s)
{
	State = s;
	//Entering state
	switch (State)
	{
	case EState::EWalking:
		GetCharacterMovement()->MaxWalkSpeed = NormalMaxWalkSpeed;
		break;
	case EState::EHolding:
		GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxCustomMovementSpeed;
		break;
	}
}

EState ACharacter_B::GetState() const
{
	return State;
}

void ACharacter_B::TryPunch()
{
	if (GetState() != EState::EWalking)
		return;

	if (!PunchComponent) { BError("No Punch Component for player %s", *GetNameSafe(this)); return; }

	PunchComponent->SetIsCharging(true);
}
UNiagaraComponent* ACharacter_B::GetChargeParticle() const
{
	return PS_Charge;
}

float ACharacter_B::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UBarrel_DamageType_B::StaticClass()))
	{
		//Does a lot of ApplyDamageMomentum manually, so we can do it to the mesh instead of movementcomponent
		UDamageType const* const DmgTypeCDO = DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>();
		float const ImpulseScale = DmgTypeCDO->DamageImpulse;
		FHitResult Hit;
		FVector ImpulseDir;
		DamageEvent.GetBestHitInfo(this, nullptr, Hit, ImpulseDir);
		bool const bMassIndependentImpulse = !DmgTypeCDO->bScaleMomentumByMass;

		CheckFall(ImpulseDir * ImpulseScale);
		//GetMesh()->AddImpulse(ImpulseDir * ImpulseScale, ForceSocketName, bMassIndependentImpulse);
		//ApplyDamageMomentum(DamageAmount, DamageEvent, nullptr, DamageCauser);
	}

	if (HurtSound)
	{
		float volume = 1.f;
		UGameInstance_B* GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (GameInstance)
			volume *= GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume();

		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			HurtSound,
			GetActorLocation(),
			volume
		);
	}
	return DamageAmount;
}