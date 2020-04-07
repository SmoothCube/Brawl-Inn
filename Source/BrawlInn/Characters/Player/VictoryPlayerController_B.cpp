// Fill out your copyright notice in the Description page of Project Settings.

#include "VictoryPlayerController_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "System/GameModes/VictoryGameMode_B.h"

void AVictoryPlayerController_B::BeginPlay()
{
	Super::BeginPlay();

	VictoryGameMode = Cast<AVictoryGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	check(IsValid(VictoryGameMode));
}

void AVictoryPlayerController_B::FaceButtonBottomPressed()
{
	VictoryGameMode->StartFadeToScore();
}
