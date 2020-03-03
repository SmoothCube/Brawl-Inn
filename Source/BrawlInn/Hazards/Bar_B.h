// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/Useable_B.h"
#include "Containers/Queue.h"
#include "Bar_B.generated.h"

class UBarMeshComponent_B;
class AAICharacter_B;
class USoundCue;
class AAIDropPoint_B;
class AItem_B;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorOpen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorClosed);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDeliverStart, AItem_B*, AAICharacter_B*);

UENUM(BlueprintType)
enum class EBarDropLocationType : uint8
{
	Stool = 0,
	Tankard
};

USTRUCT()
struct FDropLocations
{
	GENERATED_BODY()
	
	EBarDropLocationType Type;

	void AddBack(AAIDropPoint_B* DropPoint)
	{
		List.Add(DropPoint);
	}

	AAIDropPoint_B* RemoveFront()
	{
		if (List.Num() == 0)
			return nullptr;
		
		AAIDropPoint_B* DropPoint = List[0];
		List.RemoveAt(0);
		return DropPoint;
	}
	
	AAIDropPoint_B* PeekFront()
	{
		if (List.Num() == 0)
			return nullptr;
		return List[0];
	}
	
private:
	TArray<AAIDropPoint_B*> List;
};


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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UBarMeshComponent_B* House;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* Door;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* ItemSpawnLocation;

	// ********** Tankards **********
public:
	void StartTimerForNextTankard();

protected:
	void SpawnTankard();

	UPROPERTY(EditDefaultsOnly, Category="Variables|Tankard")
	FName WaiterTag = "Waiter";

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
	void AddDropLocation(EBarDropLocationType Type, AAIDropPoint_B* DropPoint);

	FOnDeliverStart OnDeliverStart;

protected:
	void SpawnStool();

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Stool")
		FName StoolReplacerTag = "StoolReplacerTag";

	UPROPERTY(EditAnywhere, Category = "Variables|Stool")
		TSubclassOf<AItem_B> BP_Stool;

	UPROPERTY(EditAnywhere, Category = "Variables|Stool")
		float MinStoolSpawnTimer = 2.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Stool")
		float MaxStoolSpawnTimer = 10.f;

	FTimerHandle TH_NextStoolTimer;

public:

	// ********** Misc **********
	FDropLocations* GetDropLocations(AAICharacter_B* Character);

	TMap<AAICharacter_B*, FDropLocations> DropLocationMap;

	int CurrentWaiterIndex = 0;
	TArray<AAICharacter_B*> AvailableWaiters;

	int CurrentStoolReplacerIndex = 0;
	TArray<AAICharacter_B*> AvailableStoolReplacer;

};
