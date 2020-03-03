// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter_B.h"
#include "System/GameModes/MainGameMode_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void AAICharacter_B::BeginPlay()
{
	Super::BeginPlay();

	StartTransform = GetActorTransform();
}

void AAICharacter_B::FellOutOfWorld(const UDamageType& dmgType)
{
	SetActorTransform(StartTransform);
}
