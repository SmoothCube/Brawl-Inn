// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/Useable_B.h"
#include "Bar_B.generated.h"

class UBarMeshComponent_B;
class USoundCue;

UCLASS()
class BRAWLINN_API ABar_B : public AActor
{
	GENERATED_BODY()

public:
	ABar_B();

	UPROPERTY(VisibleAnywhere)
		UBarMeshComponent_B* House;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		FName ItemSocket;

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<AUseable_B>> BP_Useables;

	UPROPERTY(EditAnywhere)
		USoundCue* ItemSpawnSound;

	UPROPERTY(EditAnywhere)
		float MinSpawnTimer = 2.f;

	UPROPERTY(EditAnywhere)
		float MaxSpawnTimer = 10.f;

private:

	void SpawnUseable();
	
	UFUNCTION()
	void StartTimerForNextSpawn();
	FTimerHandle TH_NextSpawnTimer;
};
