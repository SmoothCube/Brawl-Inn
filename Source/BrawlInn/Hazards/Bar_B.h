// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/Useable_B.h"
#include "Containers/Queue.h"
#include "Bar_B.generated.h"

class UBarMeshComponent_B;
class USoundCue;
class UStaticMeshComponent;
class AAIDropPoint_B;
class AItem_B;

UCLASS()
class BRAWLINN_API ABar_B : public AActor
{
	GENERATED_BODY()

public:
	ABar_B();

	UPROPERTY(VisibleAnywhere)
		UBarMeshComponent_B* House;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* Door;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* ItemSpawnLocation;

protected:
	virtual void BeginPlay() override;

	/// **** Tankards ****
	UPROPERTY(EditAnywhere, Category = "Variables|Tankard")
		FName ItemSocket = FName("Item");

	UPROPERTY(EditAnywhere, Category = "Variables|Tankard")
		TArray<TSubclassOf<AUseable_B>> BP_Useables;

	UPROPERTY(EditAnywhere, Category = "Variables|Tankard")
		USoundCue* TankardSpawnSound;

	UPROPERTY(EditAnywhere, Category = "Variables|Tankard")
		float MinTankardSpawnTimer = 2.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Tankard")
		float MaxTankardSpawnTimer = 10.f;

	FTimerHandle TH_NextTankardTimer;

	void SpawnTankard();
	
	UFUNCTION()
	void StartTimerForNextTankard();

	/// **** Stools ****
public:
	TQueue<AAIDropPoint_B*> StoolDropLocations;

	UPROPERTY()
	AItem_B* StoolToDeliver = nullptr;

protected:

	UPROPERTY(EditAnywhere, Category = "Variables|Stool")
		TSubclassOf<AItem_B> BP_Stool;

	UPROPERTY(EditAnywhere, Category = "Variables|Stool")
		float MinStoolSpawnTimer = 2.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Stool")
		float MaxStoolSpawnTimer = 10.f;

	FTimerHandle TH_NextStoolTimer;

	void SpawnStool();

	UFUNCTION()
		void StartTimerForNextStool();
};
