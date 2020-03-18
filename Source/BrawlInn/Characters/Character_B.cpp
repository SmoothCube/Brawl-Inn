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
#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"

#include "BrawlInn.h"
#include "System/GameInstance_B.h"
#include "Components/PunchComponent_B.h"
#include "System/Camera/GameCamera_B.h"
#include "System/GameModes/MainGameMode_B.h"
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
	PunchComponent->SetupAttachment(GetCapsuleComponent());
	PunchComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->MaxWalkSpeed = 1400.f;
	GetCharacterMovement()->MaxAcceleration = 3000.f;
	GetCharacterMovement()->BrakingFrictionFactor = 1;
	GetCharacterMovement()->GroundFriction = 3;

	GetCapsuleComponent()->SetCapsuleHalfHeight(91.f);
	GetCapsuleComponent()->SetCapsuleRadius(60.f);
	GetCapsuleComponent()->SetCollisionProfileName("Capsule");

	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	PS_Stun = CreateDefaultSubobject<UNiagaraComponent>("Stun Particle System");
	PS_Stun->SetupAttachment(GetMesh());
	PS_Stun->SetRelativeLocation(FVector(0.000000, -20.000000, 180.000000));

	PS_Charge = CreateDefaultSubobject<UNiagaraComponent>("Charge Particle System");
	PS_Charge->SetupAttachment(GetMesh(), "PunchCollisionHere");
}

void ACharacter_B::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ACharacter_B::OnCapsuleOverlapBegin);

	NormalMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	//caches mesh transform to reset it every time player gets up.
	RelativeMeshTransform = GetMesh()->GetRelativeTransform();

	PS_Stun->Deactivate();
	PS_Charge->Deactivate();

	GameCamera = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()))->GetGameCamera();
}

void ACharacter_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void ACharacter_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetState() == EState::EFallen)
	{
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), FindMeshLocation(), DeltaTime, 50));
	}
	else if (GetState() != EState::EBeingHeld && GetState() != EState::EPoweredUp)
	{
		HandleMovement(DeltaTime);
	}

	//	BWarn("Mesh Parent: %s, Capsule Parent: %s", *GetNameSafe(GetMesh()->GetAttachParent()), *GetNameSafe(GetCapsuleComponent()->GetAttachParent()))

}

void ACharacter_B::SetInputVectorX(const float X)
{
	InputVector.X = X;
}

void ACharacter_B::SetInputVectorY(const float Y)
{
	InputVector.Y = Y;
}

void ACharacter_B::HandleMovement(float DeltaTime)
{
	if (PunchComponent->GetIsPunching())
		return;

	if (InputVector.IsNearlyZero(0.1))
		return;

	//Normalizes to make sure we dont accelerate faster diagonally, but still want to allow for slower movement.
	if (InputVector.SizeSquared() >= 1.f)
		InputVector.Normalize();

	if (GameCamera)
	{
		AddMovementInput(GameCamera->GetActorForwardVector(), InputVector.X);
		AddMovementInput(GameCamera->GetActorRightVector(), InputVector.Y);
	}

	if (InputVector.SizeSquared() > 0)
	{
		FVector vec = GameCamera->GetActorForwardVector().ToOrientationRotator().RotateVector(InputVector);
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), vec.ToOrientationRotator(), DeltaTime, RotationInterpSpeed));
	}
}

void ACharacter_B::CheckFall(FVector MeshForce)
{
	if (!PunchComponent || PunchComponent->GetIsPunching() || bIsInvulnerable)
		return;
	Fall(MeshForce, FallRecoveryTime);
}

void ACharacter_B::Fall(FVector MeshForce, float RecoveryTime)
{
//	if (GetCharacterMovement()->IsFalling())	//Had to remove this but should probably be here? idk does this even work properly
//		return;									//yes i know i wrote it

	BWarn("Character Fell!");
	if (HoldComponent && HoldComponent->IsHolding())
		HoldComponent->Drop();

	SetState(EState::EFallen);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	//	GetMesh()->SetCollisionProfileName("Ragdoll");

	GetMesh()->AddImpulse(MeshForce, ForceSocketName, false);	//TODO make the bone dynamic instead of a variable

	if (RecoveryTime >= 0 && bIsAlive)
		GetWorld()->GetTimerManager().SetTimer(TH_FallRecoverTimer, this, &ACharacter_B::StandUp, RecoveryTime, false);
}

void ACharacter_B::StandUp()
{
	if (!bIsAlive)
		return;

	SetActorLocation(FindMeshGroundLocation());

	//if (GetCharacterMovement()->IsFalling() )// && !(GetCapsuleComponent()->GetCollisionProfileName() == "Capsule-Thrown"))
	//{
	//	BWarn("Character is falling! Cant stand up!");
	//	return;
	//}


	GetCapsuleComponent()->SetCollisionProfileName("Capsule");
	
	if (IsValid(GetMesh()))
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionProfileName("CharacterMesh");
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		GetMesh()->SetRelativeTransform(RelativeMeshTransform);

		//Saves snapshot for blending to animation
		GetMesh()->GetAnimInstance()->SavePoseSnapshot("Ragdoll");
	}
	else
	{
		BError("Mesh invalid for character %s", *GetNameSafe(this)); //This was the crash we thought was below
	}

	if(IsValid(GetMovementComponent()))
		GetMovementComponent()->StopMovementImmediately();	//CRASH HERE -E

	SetState(EState::EWalking);
	StunAmount = 0;
}

FVector ACharacter_B::FindMeshLocation() const
{
	const FVector MeshLoc = GetMesh()->GetSocketLocation("pelvis");
	FHitResult Hit;
	const bool bDidHit = GetWorld()->LineTraceSingleByChannel(Hit, MeshLoc + FVector(0, 0, 0), MeshLoc + FVector(0, 0, -250), ECollisionChannel::ECC_Visibility);

	if (bDidHit)
	{
		//	BWarn("Hit Actor: %s, Component: %s", *GetNameSafe(Hit.GetActor()), *GetNameSafe(Hit.GetComponent()));
		return (Hit.Location - RelativeMeshTransform.GetLocation());
	}
	//		BWarn("Did not hit");
	return (MeshLoc - RelativeMeshTransform.GetLocation());
}

FVector ACharacter_B::FindMeshGroundLocation() const
{
	const FVector MeshLoc = GetMesh()->GetSocketLocation("pelvis");

	FHitResult Hit;
	const bool bDidHit = GetWorld()->LineTraceSingleByChannel(Hit, MeshLoc + FVector(0, 0, 0), MeshLoc + FVector(0, 0, -1000), ECollisionChannel::ECC_Visibility);

	if (bDidHit)
	{
		//BWarn("Hit Actor: %s, Component: %s", *GetNameSafe(Hit.GetActor()), *GetNameSafe(Hit.GetComponent()))
		return (Hit.Location - RelativeMeshTransform.GetLocation());
	}
	//	else
		//	BWarn("Did not hit");
	return (MeshLoc - RelativeMeshTransform.GetLocation());
}

void ACharacter_B::PickedUp_Implementation(ACharacter_B* Player)
{

	HoldingCharacter = Player;
	SetActorLocation(GetActorLocation() + HoldOffset);
	SetActorLocation(FindMeshGroundLocation());
	GetMovementComponent()->StopMovementImmediately();
	SetState(EState::EBeingHeld);
	GetWorld()->GetTimerManager().ClearTimer(TH_FallRecoverTimer);

	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepWorldTransform);
	GetCapsuleComponent()->SetCollisionProfileName("Capsule");

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionProfileName("CharacterMesh");

	GetCharacterMovement()->StopActiveMovement();
	GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	GetMesh()->SetRelativeTransform(RelativeMeshTransform);
	SetActorRotation(HoldRotation);

}

void ACharacter_B::Dropped_Implementation()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Fall(FVector::ZeroVector, FallRecoveryTime);
	//	GetMesh()->SetSimulatePhysics(true); //should be unnecceCharacterMeshsary
	HoldingCharacter = nullptr;
	SetActorRotation(FRotator(0, 0, 0));
}

void ACharacter_B::Use_Implementation()
{
	FVector TargetLocation = HoldingCharacter->GetActorForwardVector();
	//AddActorLocalOffset(TargetLocation * 250);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (IsValid(HoldingCharacter) && IsValid(HoldingCharacter->ThrowComponent))
	{
		HoldingCharacter->ThrowComponent->AimAssist(TargetLocation);
		float ImpulseStrength = 0.f;
		IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(this);
		if (Interface)
		{
			ImpulseStrength = Interface->Execute_GetThrowStrength(this, HoldingCharacter->GetChargeLevel());
		}
		Fall(TargetLocation * ImpulseStrength, FallRecoveryTime);
		HoldingCharacter = nullptr;
	}

	GetWorld()->GetTimerManager().SetTimer(TH_FallCollisionTimer, [&]()
		{
			GetCapsuleComponent()->SetCollisionProfileName(FName("Capsule-Thrown"));
		}, TimeBeforeThrowCollision, false);

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

float ACharacter_B::GetThrowStrength_Implementation(EChargeLevel level) const
{
	switch (level)
	{
	case EChargeLevel::EChargeLevel1:
		return Charge1ThrowStrength;

	case EChargeLevel::EChargeLevel2:
		return Charge2ThrowStrength;

	case EChargeLevel::EChargeLevel3:
		return Charge3ThrowStrength;
	default:
		return 0;
	}
}
void ACharacter_B::AddStun(const int Strength)
{
	if (StunAmount == PunchesToStun - 1)
	{
		StunAmount += Strength;
		return;
	}
	StunAmount += Strength;
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

	if (InvisibleMat && !bHasShield)
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

void ACharacter_B::Die()
{
	Fall();
	bIsAlive = false;
}

void ACharacter_B::TryStartCharging()
{
	if (GetState() != EState::EWalking)
		return;

	if (!PunchComponent) { BError("No Punch Component for player %s", *GetNameSafe(this)); return; }

	SetIsCharging(true);
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

void ACharacter_B::OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	ACharacter_B* HitCharacter = Cast<ACharacter_B>(OtherActor);
	UCapsuleComponent* HitComponent = Cast<UCapsuleComponent>(OtherComp);
	if (HitComponent && HitCharacter && !HitCharacter->IsInvulnerable() && (GetCapsuleComponent()->GetCollisionProfileName() == "Capsule-Thrown"))
	{
		if (HitCharacter->HasShield())
		{
			HitCharacter->RemoveShield();
		}
		else
		{
			HitCharacter->GetCharacterMovement()->AddImpulse(GetVelocity());
			HitCharacter->CheckFall(GetVelocity());
			if (IsValid(HitCharacter) && IsValid(HitCharacter->GetController()))
				UGameplayStatics::ApplyDamage(HitCharacter, 1, HitCharacter->GetController(), this, UDamageType::StaticClass());
		}
	}

}


FRotator& ACharacter_B::GetHoldRotation()
{
	return HoldRotation;
}

EChargeLevel ACharacter_B::GetChargeLevel()
{
	return ChargeLevel;
}

void ACharacter_B::SetChargeLevel(EChargeLevel chargeLevel)
{

	ChargeLevel = chargeLevel;
	bool ShouldPlaySound = true;
	float SoundPitch = 1.0f;
	bool ShouldPlayVibration = true;
	float VibrationStrength = 1.0f;

	switch (ChargeLevel)
	{
	case EChargeLevel::EChargeLevel1:
		RotationInterpSpeed = Charge1RotSpeed;
		GetCharacterMovement()->MaxWalkSpeed = Charge1MoveSpeed;
		GetCharacterMovement()->Velocity = GetVelocity().GetClampedToMaxSize(Charge1MoveSpeed);
		ShouldPlaySound = false;
		SoundPitch = 0.8f;
		break;
	case EChargeLevel::EChargeLevel2:
		RotationInterpSpeed = Charge2RotSpeed;
		GetCharacterMovement()->MaxWalkSpeed = Charge2MoveSpeed;
		GetCharacterMovement()->Velocity = GetVelocity().GetClampedToMaxSize(Charge2MoveSpeed);
		SoundPitch = 1.0f;
		break;
	case EChargeLevel::EChargeLevel3:
		RotationInterpSpeed = Charge3RotSpeed;
		GetCharacterMovement()->MaxWalkSpeed = Charge3MoveSpeed;
		GetCharacterMovement()->Velocity = GetVelocity().GetClampedToMaxSize(Charge3MoveSpeed);
		SoundPitch = 1.2f;
		break;
	default:
		RotationInterpSpeed = NormalRotationInterpSpeed;
		GetCharacterMovement()->MaxWalkSpeed = NormalMaxWalkSpeed;
		ShouldPlaySound = false;
		break;
	}


	if (ShouldPlaySound && ChargeLevelSound)
	{
		float volume = 1.f;
		UGameInstance_B* GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (GameInstance)
		{
			volume *= GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume();
		}
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ChargeLevelSound,
			GetActorLocation(),
			volume,
			SoundPitch
		);
	}
}

void ACharacter_B::SetIsCharging(bool Value)
{
	bIsCharging = Value;
}

bool ACharacter_B::IsCharging() const
{
	return bIsCharging;
}