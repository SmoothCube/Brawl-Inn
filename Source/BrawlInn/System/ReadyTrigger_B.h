// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Characters/Player/PlayerInfo.h"
#include "ReadyTrigger_B.generated.h"

class AMenuGameMode_B;

DECLARE_MULTICAST_DELEGATE(FOnOverlapChange);

UCLASS()
class BRAWLINN_API AReadyTrigger_B : public ATriggerBox
{
	GENERATED_BODY()

public:
		AReadyTrigger_B();

protected:

	FOnOverlapChange OnReadyOverlapChange;

	virtual void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
		void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
		void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void PrepareStartGame();

	FTimerHandle TH_StartTimer;

	TArray<FPlayerInfo> PlayerInfos;

	UPROPERTY()
	AMenuGameMode_B* GameMode = nullptr;

};
