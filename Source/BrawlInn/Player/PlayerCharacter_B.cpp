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

// Called to bind functionality to input
void APlayerCharacter_B::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

