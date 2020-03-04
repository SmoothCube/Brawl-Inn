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
	
	FDropLocations()
	{
		
	}
	FDropLocations(EBarDropLocationType InType)
	{
		Type = InType;
	}

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
	
	EBarDropLocationType Type = EBarDropLocationType::Stool;
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
		UStaticMeshComponent* House;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* Door;

	// ********** Tankards **********
	
	void GiveRandomTankard(AAICharacter_B* Waiter);

	UPROPERTY(EditAnywhere, Category = "Variables|Tankard")
		TArray<TSubclassOf<AUseable_B>> BP_Useables;

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Tankard")
		FName WaiterTag = "Waiter";

	// ********** Stool Respawning **********

public:
	void AddDropLocation(EBarDropLocationType Type, AAIDropPoint_B* DropPoint);

	FOnDeliverStart OnDeliverStart;

protected:

	UPROPERTY(EditAnywhere, Category = "Variables|Stool")
		TSubclassOf<AItem_B> BP_Stool;

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Stool")
		FName StoolReplacerTag = "StoolReplacer";
public:

	// ********** Misc **********
	FDropLocations* GetDropLocations(AAICharacter_B* Character);

	TMap<AAICharacter_B*, FDropLocations> DropLocationMap;

	int CurrentWaiterIndex = 0;
	TArray<AAICharacter_B*> AvailableWaiters;

	int CurrentStoolReplacerIndex = 0;
	TArray<AAICharacter_B*> AvailableStoolReplacer;

};
