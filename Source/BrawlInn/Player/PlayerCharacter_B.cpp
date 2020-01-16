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
	FireDamageComponent->FireHealthIsZero_D.AddDynamic(this, &APlayerCharacter_B::Fall);
}

float APlayerCharacter_B::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UFire_DamageType_B::StaticClass()))
	{
		FireDamageComponent->FireDamageStart_D.Broadcast();
	}
	else if (DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UBarrel_DamageType_B::StaticClass()))
	{
		//DamageEvent.IsOfType();
		
		ApplyDamageMomentum(DamageAmount, DamageEvent, nullptr, DamageCauser);
		
	}
	else
	{
		PlayerController->HealthComponent->TakeDamage(DamageAmount);
	}
	return DamageAmount;
}

void APlayerCharacter_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!(State == EState::EFallen))
	{
		if (State == EState::EWalking)
			HandleMovement(DeltaTime);
		else if (State == EState::EHolding)
			HandleMovementHold();
		HandleRotation();
	}
}

void APlayerCharacter_B::HandleRotation()
{
	if (RotationVector.Size() > 0.1)
	{
		RotationVector.Normalize();
		SetActorRotation(RotationVector.ToOrientationRotator());
	}
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
		if (CurrentFallTime >= TimeBeforeFall)
		{
			Fall();
		}
		else if (Speed >= GetMovementComponent()->GetMaxSpeed() * FallLimitMultiplier)
		{
			Fall();
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
	GetCharacterMovement()->MaxWalkSpeed = 400.f;

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
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->AddImpulse(GetMovementComponent()->Velocity, NAME_None, true);

		//Decides which parts of the controller to vibrate. Overkill? probably but i wanted to test it.
		bool RU, RD, LU, LD;
		RU = RD = LU = LD = true; // false

		if (PlayerController)
			PlayerController->PlayControllerVibration(1.f, 0.5f, LU, LD, RU, RD);

		GetWorld()->GetTimerManager().SetTimer(TH_RecoverTimer, this, &APlayerCharacter_B::StandUp, RecoveryTime, false);
	}
}

void APlayerCharacter_B::StandUp()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		//UE_LOG(LogTemp, Warning, TEXT("[APlayerCharacter_B::StandUp] Getting Up!"));
		State = EState::EWalking;
		GetMovementComponent()->StopMovementImmediately();
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCapsuleComponent()->SetWorldLocation(GetMesh()->GetRelativeTransform().GetLocation());
		FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
		GetMesh()->AttachToComponent(GetRootComponent(), Rule);
		GetMesh()->SetRelativeTransform(RelativeMeshTransform);
		AddActorWorldOffset(FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	}
}

APlayerController_B* APlayerCharacter_B::GetPlayerController_B() const
{
	return PlayerController;
}

void APlayerCharacter_B::FellOutOfWorld(const UDamageType& dmgType)
{
	UGameplayStatics::ApplyDamage(this, FellOutOfWorldDamageAmount, PlayerController, this, dmgType.StaticClass());
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

void APlayerCharacter_B::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PlayerController = Cast<APlayerController_B>(NewController);

}

void APlayerCharacter_B::PunchButtonPressed()
{
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

	//Calculates force vector
	FVector Direction = GetActorLocation() - Origin;
	Direction.Normalize();
	Direction*=DamageType->DamageImpulse;

	GetCharacterMovement()->AddImpulse(Direction);
}
