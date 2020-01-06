// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter_B.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharacter_B::APlayerCharacter_B()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayerCharacter_B::BeginPlay()
{
	Super::BeginPlay();
	//SetActorRelativeRotation(FRotator(0,90,0));
}

// Called every frame
void APlayerCharacter_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	InputVector.Normalize();
	GetMovementComponent()->AddInputVector(InputVector*MaxSpeed);

	SetActorRotation(RotationVector.Rotation());
}
