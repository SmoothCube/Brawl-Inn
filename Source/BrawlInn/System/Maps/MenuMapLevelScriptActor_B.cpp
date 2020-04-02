// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuMapLevelScriptActor_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "System/GameInstance_B.h"
#include "System/GameModes/MenuGameMode_B.h"

AMenuMapLevelScriptActor_B::AMenuMapLevelScriptActor_B()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMenuMapLevelScriptActor_B::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	checkf(GameMode, TEXT("Wrong GameMode in MenuMapStreamed!"));
	const FLatentActionInfo Info(0, 0, TEXT("PostLevelLoad"), GameMode);
	UGameplayStatics::LoadStreamLevel(GetWorld(), GameMapName,true,false,Info);
}