// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/Useable_B.h"
#include "Containers/Queue.h"
#include "Bar_B.generated.h"

class UBarMeshComponent_B;
class USoundCue;
class AAIDropPoint_B;
class AItem_B;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorOpen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorClosed);

UCLASS()
class BRAWLINN_API ABar_B : public AActor
{
	GENERATED_BODY()
public:
	ABar_B();

	// ********** AActor **********
protected:
	virtual void BeginPlay() override;

	// ********** Bar **********
public:
	UFUNCTION(BlueprintImplementableEvent)
		void OpenDoor();

	UFUNCTION(BlueprintImplementableEvent)
		void CloseDoor();

	USceneComponent* GetItemSpawnLocation() const;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FOnDoorOpen OnDoorOpen_D;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FOnDoorClosed OnDoorClosed_D;

protected:
	UPROPERTY(VisibleAnywhere)
		UBarMeshComponent_B* House;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* Door;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* ItemSpawnLocation;

	// ********** Tankards **********
	void StartTimerForNextTankard();

	void SpawnTankard();

	UPROPERTY(EditAnywhere, Category = "Variables|Tankard")
		FName ItemSocket = FName("Item");

	UPROPERTY(EditAnywhere, Category = "Variables|Tankard")
		TArray<TSubclassOf<AUseable_B>> BP_Useables;

	UPROPERTY(EditAnywhere, Category = "Variables|Tankard")
		float MinTankardSpawnTimer = 2.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Tankard")
		float MaxTankardSpawnTimer = 10.f;

	FTimerHandle TH_NextTankardTimer;

	UPROPERTY(EditAnywhere, Category = "Variables|Tankard|Sound")
		USoundCue* TankardSpawnSound;

	UPROPERTY(EditAnywhere, Category = "Variables|Tankard|Vfx")
		UNiagaraSystem* TankardSpawnParticle;

	// ********** Stool Respawning **********

public:
	void StartTimerForNextStool();
	
	TQueue<AAIDropPoint_B*>& GetStoolDropLocations();

protected:
	void SpawnStool();

	TQueue<AAIDropPoint_B*> StoolDropLocations;

	UPROPERTY(EditAnywhere, Category = "Variables|Stool")
		TSubclassOf<AItem_B> BP_Stool;

	UPROPERTY(EditAnywhere, Category = "Variables|Stool")
		float MinStoolSpawnTimer = 2.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Stool")
		float MaxStoolSpawnTimer = 10.f;

	FTimerHandle TH_NextStoolTimer;
};
