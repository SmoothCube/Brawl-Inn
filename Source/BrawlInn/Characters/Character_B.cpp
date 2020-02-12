// Fill out your copyright notice in the Description page of Project Settings.

#include "Character_B.h"
#include "BrawlInn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"
#include "Sound/SoundCue.h"
#include "NiagaraComponent.h"

#include "System/Interfaces/ControllerInterface_B.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/GameMode_B.h"
#include "System/Camera/GameCamera_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "Components/HealthComponent_B.h"
#include "Characters/Player/PlayerController_B.h"
#include "Items/Throwable_B.h"
#include "Animations/PlayerAnimInstance_B.h"
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

	GetCharacterMovement()->MaxWalkSpeed = 2000;
	GetCharacterMovement()->MaxAcceleration = 800;
	GetCharacterMovement()->BrakingFrictionFactor = 1;
	GetCharacterMovement()->GroundFriction = 3;

	PS_Stun = CreateDefaultSubobject<UNiagaraComponent>("Stun Particle System");
	PS_Stun->SetupAttachment(GetMesh());

	PS_Charge = CreateDefaultSubobject<UNiagaraComponent>("Charge Particle System");
	PS_Charge->SetupAttachment(GetMesh(), "PunchCollisionHere");

}

void ACharacter_B::BeginPlay()
{
	Super::BeginPlay();
	NormalMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	//caches mesh transform to reset it every time player gets up.
	RelativeMeshTransform = GetMesh()->GetRelativeTransform();
	OnTakeRadialDamage.AddDynamic(this, &ACharacter_B::OnRadialDamageTaken);

	PS_Stun->Deactivate();
	PS_Charge->Deactivate();
	MakeInvulnerable(1.0f);

	for (TActorIterator<AGameCamera_B> itr(GetWorld()); itr; ++itr)
	{
		GameCamera = *itr;
	}
}

void ACharacter_B::Tick(float DeltaTime)
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
		if (!PunchComponent->bIsPunching)
			CheckFall(DeltaTime);
		if (!(GetState() == EState::EStunned))
		{
			HandleMovement(DeltaTime);
			HandleRotation(DeltaTime);
		}
	}
}

void ACharacter_B::HandleRotation(float DeltaTime)
{
	if (InputVector.Size() > 0.1)
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), InputVector.ToOrientationRotator(), DeltaTime, 10.f));
}

void ACharacter_B::HandleMovement(float DeltaTime)
{
	//Normalizes to make sure we dont accelerate faster diagonally, but still want to allow for slower movement.
	if (InputVector.SizeSquared() >= 1.f)
		InputVector.Normalize();
	GetMovementComponent()->AddInputVector(InputVector);
}

void ACharacter_B::CheckFall(float DeltaTime)
{
	float Speed = GetMovementComponent()->Velocity.Size();
	if (Speed >= NormalMaxWalkSpeed * FallLimitMultiplier)
	{
		Fall(PunchedRecoveryTime);
		MakeInvulnerable(InvulnerabilityTime + PunchedRecoveryTime);
	}
}

void ACharacter_B::Fall(float RecoveryTime)
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
		FVector Force = GetMovementComponent()->Velocity ;
		GetMesh()->AddImpulse(Force, ForceSocketName, true);	//TODO make the bone dynamic instead of a variable
		BWarn("Adding force of %s to %s", *Force.ToString(), *ForceSocketName.ToString());
		if (PlayerController)
			PlayerController->PlayControllerVibration(1.f, 0.5f, true, true, true, true);
		if (RecoveryTime >= 0)
			GetWorld()->GetTimerManager().SetTimer(TH_RecoverTimer, this, &ACharacter_B::StandUp, RecoveryTime, false);
	}
}

FVector ACharacter_B::FindMeshLocation()
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

float ACharacter_B::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UBarrel_DamageType_B::StaticClass()))
	{
		ApplyDamageMomentum(DamageAmount, DamageEvent, nullptr, DamageCauser);
	}

	return DamageAmount;
}

void ACharacter_B::StandUp()
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
void ACharacter_B::MakeInvulnerable(float ITime)
{
	bIsInvulnerable = true;

	if (InvulnerableMat)
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
	if (!bHasShield)
		return;

	bHasShield = false;

	if (ShieldMat)
		GetMesh()->SetMaterial(SpecialMaterialIndex, InvisibleMat);
}

void ACharacter_B::AddStun(int Strength)
{
	if (!(GetState() == EState::EStunned))
	{
		StunAmount += Strength;
		if (StunAmount >= PunchesToStun)
		{
			SetState(EState::EStunned);
		}
		GetWorld()->GetTimerManager().SetTimer(TH_StunTimer, this, &ACharacter_B::RemoveStun, StunTime, false);
	}
}

void ACharacter_B::RemoveStun()
{
	if (GetState() == EState::EStunned)
	{
		SetState(EState::EWalking);
	}
	StunAmount = 0;
}

void ACharacter_B::PickedUp_Implementation(ACharacter_B* Player)
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

void ACharacter_B::Dropped_Implementation()
{
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DetachFromActor(rules);
	Fall(PunchedRecoveryTime);
	GetMesh()->SetSimulatePhysics(true);
	HoldingPlayer = nullptr;
	SetActorRotation(FRotator(0, 0, 0));
	CurrentHoldTime = 0.f;
}

void ACharacter_B::Use_Implementation()
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

void ACharacter_B::SetState(EState s)
{
	//on leaving state
	switch (State)
	{
	case EState::EStunned:
		if (IsValid(PS_Stun))
			PS_Stun->DeactivateImmediate();
		CurrentHoldTime = 0.f;

		break;
	}

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
	case EState::EStunned:
		if (IsValid(PS_Stun))
			PS_Stun->Activate();
		break;
	}
}

EState ACharacter_B::GetState() const
{
	return State;
}

bool ACharacter_B::IsHeld_Implementation() const
{
	if (HoldingPlayer)
		return true;
	return false;
}

APlayerController_B* ACharacter_B::GetPlayerController_B() const
{
	return PlayerController;
}

void ACharacter_B::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ACharacter_B::PunchButtonPressed()
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

void ACharacter_B::BreakFreeButtonMash()
{
	CurrentHoldTime += 0.01;
}

void ACharacter_B::BreakFree()
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
		HoldingPlayer->AddStun(PunchesToStun);
		HoldingPlayer = nullptr;
	}
}

void ACharacter_B::OnRadialDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	BWarn("Radial Damage Taken!");
	if (!bIsInvulnerable)
	{
		if (bHasShield)
		{
			RemoveShield();
		}
		else
		{
			//Calculates force vector
			//FVector Direction = GetActorLocation() - Origin;
			//Direction.Normalize();
			//Direction *= DamageType->DamageImpulse;
			//GetCharacterMovement()->AddImpulse(Direction);
		}
	}
}
