// Fill out your copyright notice in the Description page of Project Settings.

#include "VictoryMapLevelScriptActor_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "System/GameModes/VictoryGameMode_B.h"
#include "System/GameInstance_B.h"

AVictoryMapLevelScriptActor_B::AVictoryMapLevelScriptActor_B()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVictoryMapLevelScriptActor_B::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	GameMode = Cast<AVictoryGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	checkf(GameMode, TEXT("Wrong GameMode in VictoryMapStreamed!"));
	const FLatentActionInfo Info(0, 0, TEXT("PostLevelLoad"), GameMode);
	UGameplayStatics::LoadStreamLevel(GetWorld(), GameMapName, true, true, Info);
}
