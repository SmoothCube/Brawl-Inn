// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter_B.h"
#include "System/MainGameMode_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void AAICharacter_B::BeginPlay()
{
	Super::BeginPlay();

	// Temporary so I can see what I'm doing while working on ai
	AMainGameMode_B* Gamemode = Cast <AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (Gamemode)
	{
		Gamemode->AddCameraFocusPoint(Cast<USceneComponent>(GetMesh()));
	}
}
