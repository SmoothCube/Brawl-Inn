// Fill out your copyright notice in the Description page of Project Settings.

#include "ReadyTrigger_B.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "Characters/Player/MenuPlayerController_B.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "SubSystems/ScoreSubSystem_B.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/MenuGameMode_B.h"
#include "UI/Widgets/CharacterSelectionOverlay_B.h"

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
	GameMode->PlayersActiveUpdated.BindUObject(this, &AReadyTrigger_B::ClearReadyTimer);
}

void AReadyTrigger_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AReadyTrigger_B::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	APlayerCharacter_B* PlayerCharacter = Cast<APlayerCharacter_B>(OtherActor);
	if (PlayerCharacter && GameMode)
	{
		GameMode->SetPlayersReady(GameMode->GetPlayersReady() + 1);

		PlayerInfos.Add(Cast<AMenuPlayerController_B>(PlayerCharacter->GetController())->GetPlayerInfo());

		GameMode->UpdateCharacterSelectionOverlay();

		if (GameMode->GetPlayersReady() >= GameMode->GetPlayersActive())
			GetWorld()->GetTimerManager().SetTimer(TH_StartTimer, this, &AReadyTrigger_B::PrepareStartGame, 3.f, false);
	}
}

void AReadyTrigger_B::ClearReadyTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(TH_StartTimer);
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

		GameMode->UpdateCharacterSelectionOverlay();

		ClearReadyTimer();
	}
}

void AReadyTrigger_B::PrepareStartGame()
{
	checkf(IsValid(GameMode), TEXT("GameMode is not valid. Can't start game!"));

	UGameInstance_B* GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	for (auto PlayerInfo : PlayerInfos)
	{
		APlayerController* Controller = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), PlayerInfo.ID);
		Controller->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->ResetScoreValues();
	}

	GameInstance->SetPlayerInfos(PlayerInfos);

	GameMode->PrepareGameStart_Delegate.Broadcast();
}