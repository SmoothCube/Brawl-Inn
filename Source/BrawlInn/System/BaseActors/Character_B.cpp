// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_B.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "BrawlInn.h"
#include "Components/HoldComponent_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "Animations/PlayerAnimInstance_B.h"
#include "Player/PlayerController_B.h"

// Sets default values
ACharacter_B::ACharacter_B()
{
	PrimaryActorTick.bCanEverTick = true;

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

}

// Called when the game starts or when spawned
void ACharacter_B::BeginPlay()
{
	Super::BeginPlay();

	NormalMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	//caches mesh transform to reset it every time player gets up.
	RelativeMeshTransform = GetMesh()->GetRelativeTransform();
	OnTakeRadialDamage.AddDynamic(this, &ACharacter_B::OnRadialDamageTaken);

}

// Called every frame
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
		GetMesh()->AddImpulse(GetMovementComponent()->Velocity, "ProtoPlayer_BIND_SpineTop_JNT_center", true);

		APlayerController_B* PlayerController = Cast<APlayerController_B>(GetController());
		if (PlayerController)
			PlayerController->PlayControllerVibration(1.f, 0.5f, true, true, true, true);

		GetWorld()->GetTimerManager().SetTimer(TH_RecoverTimer, this, &ACharacter_B::StandUp, RecoveryTime, false);
	}
	else
		BWarn("Character is in air! Fall() does not run!");
}

bool ACharacter_B::CheckFall(float DeltaTime)
{
	float Speed = GetMovementComponent()->Velocity.Size();
	if (Speed >= NormalMaxWalkSpeed * FallLimitMultiplier)
	{
		Fall(PunchedRecoveryTime);
		return true;
	}
	return false;
}

void ACharacter_B::OnRadialDamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	BWarn("Radial Damage Taken!");

	//Calculates force vector
	FVector Direction = GetActorLocation() - Origin;
	Direction.Normalize();
	Direction *= DamageType->DamageImpulse;
	GetCharacterMovement()->AddImpulse(Direction);
}

void ACharacter_B::SetState(EState s)
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

EState ACharacter_B::GetState() const
{
	return State;
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

void ACharacter_B::FellOutOfWorld(const UDamageType& dmgType)
{
	Super::FellOutOfWorld(dmgType);
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