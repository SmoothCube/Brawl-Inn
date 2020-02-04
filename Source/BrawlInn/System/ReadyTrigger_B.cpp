// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadyTrigger_B.h"
#include "BrawlInn.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "System/MenuGameMode_B.h"
#include "Kismet/GameplayStatics.h"
#include "System/GameInstance_B.h"

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

		PlayerControllerIds.Add(UGameplayStatics::GetPlayerControllerID(Cast<APlayerController>(PlayerCharacter->GetController())));

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
		PlayerControllerIds.Remove(UGameplayStatics::GetPlayerControllerID(Cast<APlayerController>(PlayerCharacter->GetController())));

		GetWorld()->GetTimerManager().ClearTimer(TH_StartTimer);
	}
}

void AReadyTrigger_B::PrepareStartGame()
{
	if (!IsValid(GameMode)) { BError("GameMode is not valid. Can't start game!"); return; }

	UGameInstance_B* GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	GameInstance->SetActivePlayerControllerIDs(PlayerControllerIds);

	GameMode->StartGame();
}