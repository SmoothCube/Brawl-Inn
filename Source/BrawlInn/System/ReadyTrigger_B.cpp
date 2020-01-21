// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadyTrigger_B.h"
#include "BrawlInn.h"
#include "Player/PlayerCharacter_B.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "System/MenuGameMode_B.h"
#include "Kismet/GameplayStatics.h"

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
	if (Cast<APlayerCharacter_B>(OtherActor) && GameMode)
	{
		BScreen("Started overlap with %s", *GetNameSafe(OtherActor));
		GameMode->PlayersReady++;

		if (GameMode->PlayersReady >= GameMode->PlayersActive)
		{
			GetWorld()->GetTimerManager().SetTimer(TH_StartTimer, GameMode, &AMenuGameMode_B::StartGame, 0.2f, false);
		}
	}
}

void AReadyTrigger_B::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	BScreen("Ended overlap with %s", *GetNameSafe(OtherActor));
	GameMode->PlayersReady--;

	GetWorld()->GetTimerManager().ClearTimer(TH_StartTimer);
}
