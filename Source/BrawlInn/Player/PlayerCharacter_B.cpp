// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter_B.h"
#include "BrawlInn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"

#include "Player/PlayerController_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "Items/Throwable_B.h"

APlayerCharacter_B::APlayerCharacter_B()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;

	HoldComponent = CreateDefaultSubobject<UHoldComponent_B>("Hold Component");
	HoldComponent->SetupAttachment(GetMesh());

	ThrowComponent = CreateDefaultSubobject<UThrowComponent_B>("Throw Component");
	
	PunchComponent = CreateDefaultSubobject<UPunchComponent_B>("PunchComponent");
	PunchComponent->SetupAttachment(GetMesh(), "PunchCollisionHere");
	PunchComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APlayerCharacter_B::BeginPlay()
{
	Super::BeginPlay();
	
	//caches mesh transform to reset it every time player gets up.
	RelativeMeshTransform = GetMesh()->GetRelativeTransform();
	//GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter_B::CapsuleBeginOverlap);
}

void APlayerCharacter_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleMovement(DeltaTime);
	HandleRotation();
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
	//Normalizes to make sure we dont move faster than max speed, but still want to allow for slower movement.
	if (InputVector.SizeSquared() >= 1.f) 
		InputVector.Normalize();
	float Speed = GetMovementComponent()->Velocity.Size();
	if (Speed >= GetMovementComponent()->GetMaxSpeed() * 0.9f)
	{
		CurrentFallTime += DeltaTime;
		float Intensity = CurrentFallTime + 0.7;
		if (!PunchComponent->bIsPunching && PlayerController)
			PlayerController->PlayDynamicForceFeedback(Intensity, 0.1f, true, true, true, true);
		if (CurrentFallTime >= TimeBeforeFall && !bHasFallen)
		{
			
			BWarn("[APlayerCharacter_B::HandleMovement] Falling! FallTime passed! Velocity: %s", *GetMovementComponent()->Velocity.ToString());
			Fall();

		}
		else if (Speed >= GetMovementComponent()->GetMaxSpeed() * FallLimitMultiplier && !bHasFallen)
		{
			BWarn("Falling! Speed over max! Velocity: %s Speed: %f", *GetMovementComponent()->Velocity.ToString(), Speed);
			Fall();
		}
	}
	else
	{
		CurrentFallTime = 0;
	}
	GetMovementComponent()->AddInputVector(InputVector);
}
void APlayerCharacter_B::Fall()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		
		//UE_LOG(LogTemp, Warning, TEXT("[APlayerCharacter_B::HandleMovement] Falling! Velocity: %s"), *GetMovementComponent()->Velocity.ToString());
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->AddImpulse(GetMovementComponent()->Velocity, NAME_None, true);

		//Decides which parts of the controller to vibrate. Overkill? probably but i wanted to test it.
		bool RU, RD, LU, LD;
		RU = RD = LU = LD = true; // false

		//FVector Vel = GetCharacterMovement()->Velocity;
		//Vel.Normalize();
		//if (Vel.X > 0.1)
		//{
		//	if (Vel.Y > 0.1)
		//		RU = true;
		//	else if (Vel.Y < -0.1)
		//		LU = true;
		//	else
		//		RU = LU = true;
		//}
		//else if (Vel.X < -0.1)
		//{
		//	if (Vel.Y > 0.1)
		//		RD = true;
		//	else if (Vel.Y < -0.1)
		//		LD = true;
		//	else
		//		RD = LD = true;
		//}
		//else
		//{
		//	if (Vel.Y > 0.1)
		//		RD = RU = true;
		//	else if (Vel.Y < -0.1)
		//		LD = RU = true;
		//	else
		//		RD = LD = RU = LU = true;
		//}

		if (PlayerController)
			PlayerController->PlayDynamicForceFeedback(1.f, 0.5f, LU, LD, RU, RD);	
		GetWorld()->GetTimerManager().SetTimer(TH_RecoverTimer,this, &APlayerCharacter_B::StandUp, RecoveryTime,false);
		bHasFallen = true;
	}
}

void APlayerCharacter_B::StandUp()
{
	//UE_LOG(LogTemp, Warning, TEXT("[APlayerCharacter_B::StandUp] Getting Up!"));
	GetMovementComponent()->StopMovementImmediately();
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetWorldLocation(GetMesh()->GetRelativeTransform().GetLocation());
	FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
	GetMesh()->AttachToComponent(GetRootComponent(), Rule);
	GetMesh()->SetRelativeTransform(RelativeMeshTransform);
	AddActorWorldOffset(FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));

	bHasFallen = false;
}

//TODO Cleanup, not in use.
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