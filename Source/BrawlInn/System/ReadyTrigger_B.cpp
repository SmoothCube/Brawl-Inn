// Fill out your copyright notice in the Description page of Project Settings.

#include "ReadyTrigger_B.h"

#include "BrawlInn.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "Characters/Player/MenuPlayerController_B.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/MenuGameMode_B.h"

AReadyTrigger_B::AReadyTrigger_B()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AReadyTrigger_B::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AReadyTrigger_B::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AReadyTrigger_B::OnEndOverlap);
	
	GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	check(GameMode);
	GameMode->PlayersActiveUpdated.BindUObject(this, &AReadyTrigger_B::ClearReadyTimer);
}

void AReadyTrigger_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(TH_StartTimer);
}

void AReadyTrigger_B::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	APlayerCharacter_B* PlayerCharacter = Cast<APlayerCharacter_B>(OtherActor);
	check(IsValid(GameMode));

	if (PlayerCharacter)
	{
		GameMode->SetPlayersReady(GameMode->GetPlayersReady() + 1);

		GameMode->UpdateCharacterSelectionOverlay();

		if (GameMode->GetPlayersReady() >= GameMode->GetPlayersActive())
			GetWorld()->GetTimerManager().SetTimer(TH_StartTimer, this, &AReadyTrigger_B::PrepareStartGame, 3, false);
	}
}

void AReadyTrigger_B::ClearReadyTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(TH_StartTimer);
}

void AReadyTrigger_B::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	BLog("OLG");
	APlayerCharacter_B* PlayerCharacter = Cast<APlayerCharacter_B>(OtherActor);
	if (PlayerCharacter && GameMode)
	{
		GameMode->SetPlayersReady(GameMode->GetPlayersReady() - 1);

		GameMode->UpdateCharacterSelectionOverlay();

		ClearReadyTimer();
	}
}

void AReadyTrigger_B::PrepareStartGame()
{
	checkf(IsValid(GameMode), TEXT("GameMode is not valid. Can't start game!"));

	UGameInstance_B* GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	checkf(IsValid(GameInstance), TEXT("GameInstance is not valid. Can't start game!"));

	TArray<AActor*> OutActors;
	GetOverlappingActors(OutActors, APlayerCharacter_B::StaticClass());

	PlayerInfos.Empty();
	
	for(auto Actor : OutActors)
	{
		PlayerInfos.Add(Cast<APlayerController_B>(Cast<APlayerCharacter_B>(Actor)->GetController())->GetPlayerInfo());
	}
	
	GameInstance->SetPlayerInfos(PlayerInfos);
	GameMode->PrepareGameStart();
}