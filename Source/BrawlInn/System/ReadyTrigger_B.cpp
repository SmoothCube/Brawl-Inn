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
}

void AReadyTrigger_B::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	APlayerCharacter_B* PlayerCharacter = Cast<APlayerCharacter_B>(OtherActor);
	if (PlayerCharacter && GameMode)
	{
		BScreen("Started overlap with %s", *GetNameSafe(OtherActor));
		GameMode->PlayersReady++;
		FPlayerInfo info;
		info.ID = UGameplayStatics::GetPlayerControllerID(Cast<APlayerController>(PlayerCharacter->GetController()));
		PlayerInfos.Add(info);

		if (GameMode->PlayersReady >= GameMode->PlayersActive)
		{
			GetWorld()->GetTimerManager().SetTimer(TH_StartTimer, this, &AReadyTrigger_B::PrepareStartGame, 0.2f, false);
		}
	}
}

void AReadyTrigger_B::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	APlayerCharacter_B* PlayerCharacter = Cast<APlayerCharacter_B>(OtherActor);
	if (PlayerCharacter && GameMode)
	{
		BScreen("Ended overlap with %s", *GetNameSafe(OtherActor));
		GameMode->PlayersReady--;
		PlayerInfos.RemoveAll([&](const FPlayerInfo& Info) {
			return Info.ID == UGameplayStatics::GetPlayerControllerID(Cast<APlayerController>(PlayerCharacter->GetController()));
			});

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