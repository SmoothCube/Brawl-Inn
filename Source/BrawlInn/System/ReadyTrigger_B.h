// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Characters/Player/PlayerInfo.h"
#include "ReadyTrigger_B.generated.h"

class AMenuGameMode_B;

UCLASS()
class BRAWLINN_API AReadyTrigger_B : public ATriggerBox
{
	GENERATED_BODY()

public:
		AReadyTrigger_B();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
		void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
		void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void PrepareStartGame();

	FTimerHandle TH_StartTimer;

	TArray<FPlayerInfo> PlayerInfos;

	AMenuGameMode_B* GameMode = nullptr;

};
