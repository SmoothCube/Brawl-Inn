// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMapLevelScriptActor_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "System/GameInstance_B.h"
#include "System/GameModes/MainGameMode_B.h"

AGameMapLevelScriptActor_B::AGameMapLevelScriptActor_B()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGameMapLevelScriptActor_B::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	checkf(GameMode, TEXT("Wrong GameMode in GameModeStreamed!"));
	const FLatentActionInfo Info(0, 0, TEXT("PostLevelLoad"), GameMode);
	UGameplayStatics::LoadStreamLevel(GetWorld(), GameMapName, true, true, Info);
}
