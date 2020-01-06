// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter_B.h"

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
	
}

// Called every frame
void APlayerCharacter_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter_B::MoveUp(float Value)
{

}

void APlayerCharacter_B::MoveRight(float Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("[APlayerController_B::MoveRight] Value: %f"), Value);
}
