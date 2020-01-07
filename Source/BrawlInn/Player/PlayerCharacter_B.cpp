// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter_B.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
//#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

APlayerCharacter_B::APlayerCharacter_B()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerCharacter_B::BeginPlay()
{
	Super::BeginPlay();

	//caches mesh transform to reset it every time player gets up.
	RelativeMeshTransform = GetMesh()->GetRelativeTransform();
}

void APlayerCharacter_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleMovement();

	SetActorRotation(RotationVector.Rotation());
}


void APlayerCharacter_B::HandleMovement()
{
	//Normalizes to make sure we dont move faster than max speed, but still want to allow for slower movement.
	if (InputVector.SizeSquared() >= 1.f) 
		InputVector.Normalize();
	
	if (GetMovementComponent()->Velocity.Size() >= GetMovementComponent()->GetMaxSpeed() * 0.9f)
	{
		if(!bHasFallen)
			Fall();
	}

	GetMovementComponent()->AddInputVector(InputVector);
}
void APlayerCharacter_B::Fall()
{
	UE_LOG(LogTemp, Warning, TEXT("[APlayerCharacter_B::HandleMovement] Falling! Velocity: %s"), *GetMovementComponent()->Velocity.ToString());

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);
	GetMovementComponent()->StopMovementImmediately();
	GetWorld()->GetTimerManager().SetTimer(TH_RecoverTimer,this, &APlayerCharacter_B::StandUp, RecoveryTime,false);
	bHasFallen = true;
}

void APlayerCharacter_B::StandUp()
{
	UE_LOG(LogTemp, Warning, TEXT("[APlayerCharacter_B::StandUp] Getting Up!"));

	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetWorldLocation(GetMesh()->GetRelativeTransform().GetLocation());
	FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
	GetMesh()->AttachToComponent(GetRootComponent(), Rule);
	GetMesh()->SetRelativeTransform(RelativeMeshTransform);
	AddActorWorldOffset(FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));

	bHasFallen = false;
}