// Fill out your copyright notice in the Description page of Project Settings.


#include "InitPawn_B.h"

// Sets default values
AInitPawn_B::AInitPawn_B()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInitPawn_B::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInitPawn_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AInitPawn_B::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

