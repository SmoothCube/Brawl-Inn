// Fill out your copyright notice in the Description page of Project Settings.

#include "ReadyTrigger_B.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

#include "BrawlInn.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/MenuGameMode_B.h"

AReadyTrigger_B::AReadyTrigger_B()
{
	PrimaryActorTick.bCanEverTick = false;
	OnActorBeginOverlap.AddDynamic(this, &AReadyTrigger_B::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AReadyTrigger_B::OnEndOverlap);
}

void AReadyTrigger_B::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	OnReadyOverlapChange.AddUObject(GameMode, &AMenuGameMode_B::UpdateNumberOfReadyPlayers);
}

void AReadyTrigger_B::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	APlayerCharacter_B* PlayerCharacter = Cast<APlayerCharacter_B>(OtherActor);
	if (PlayerCharacter && GameMode)
	{
		GameMode->SetPlayersReady(GameMode->GetPlayersReady() + 1);
		PlayerCharacter->PlayerInfo.ID = UGameplayStatics::GetPlayerControllerID(Cast<APlayerController>(PlayerCharacter->GetController()));
		PlayerInfos.Add(PlayerCharacter->PlayerInfo);

		OnReadyOverlapChange.Broadcast();

		if (GameMode->GetPlayersReady() >= GameMode->GetPlayersActive())
			GetWorld()->GetTimerManager().SetTimer(TH_StartTimer, this, &AReadyTrigger_B::PrepareStartGame, 3.f, false);
	}
}

void AReadyTrigger_B::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	APlayerCharacter_B* PlayerCharacter = Cast<APlayerCharacter_B>(OtherActor);
	if (PlayerCharacter && GameMode)
	{
		GameMode->SetPlayersReady(GameMode->GetPlayersReady() - 1);
		PlayerInfos.RemoveAll([&](const FPlayerInfo& Info) {
			return Info.ID == UGameplayStatics::GetPlayerControllerID(Cast<APlayerController>(PlayerCharacter->GetController()));
			});

		OnReadyOverlapChange.Broadcast();

		GetWorld()->GetTimerManager().ClearTimer(TH_StartTimer);
	}
}

void AReadyTrigger_B::PrepareStartGame()
{
	if (!IsValid(GameMode)) { BError("GameMode is not valid. Can't start game!"); return; }

	UGameInstance_B* GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	GameInstance->SetPlayerInfos(PlayerInfos);

	GameMode->StartGame();
}