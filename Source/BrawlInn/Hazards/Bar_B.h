// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/Useable_B.h"
#include "System/Structs/BarDropLocations.h"
#include "Bar_B.generated.h"

class UGameInstance_B;
class AIdleAICharacter_B;
class UStaticMeshComponent;
class AAICharacter_B;
class AAIDropPoint_B;
class AItem_B;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDeliverStart, AItem_B*, AAICharacter_B*);

UCLASS()
class BRAWLINN_API ABar_B : public AActor
{
	GENERATED_BODY()
public:
	ABar_B();

	// ********** AActor **********
protected:
	virtual void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ********** Components **********
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* House;

	// ********** Tankard **********
protected:
	void GiveRandomTankard(AAICharacter_B* Waiter);
	void RandomOrder();

	UPROPERTY(EditAnywhere, Category = "Variables|Tankard")
		TArray<TSubclassOf<AUseable_B>> BP_Useables;

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Tankard")
		FName WaiterTag = "Waiter";

	// ********** Stool **********

	UPROPERTY(EditAnywhere, Category = "Variables|Stool")
		TSubclassOf<AItem_B> BP_Stool;

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Stool")
		FName StoolReplacerTag = "StoolReplacer";

	// ********** Box **********

	UPROPERTY(EditAnywhere, Category = "Variables|Box")
		TSubclassOf<AItem_B> BP_Box;

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Box")
		FName BoxReplacerTag = "BoxReplacer";


	// ********** Drink ordering **********
public:
	void StartRandomOrder(float Time = -1);

	void GetOrder(AAIDropPoint_B* DropPoint);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Audio")
		USoundCue* DrinkReadySound;

	TArray<AIdleAICharacter_B*> Customers;

	FTimerHandle TH_StartOrderTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Score", meta = (Tooltip = "This value is overridden if ShouldUseSpreadSheets is enabled"))
		float TimeUntilFirstDelivery = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Score", meta = (Tooltip = "This value is overridden if ShouldUseSpreadSheets is enabled"))
		float TimeUntilDelivery = 10;

	// ********** Delivery **********
public:
	void AddDropLocation(EBarDropLocationType Type, AAIDropPoint_B* DropPoint);

	FBarDropLocations* GetDropLocations(AAICharacter_B* Character);

	FOnDeliverStart& OnDeliverStart();

protected:
	FOnDeliverStart OnDeliverStart_Delegate;

	TMap<AAICharacter_B*, FBarDropLocations> DropLocationMap;

	int CurrentWaiterIndex = 0;
	TArray<AAICharacter_B*> Waiters;

	int CurrentStoolReplacerIndex = 0;
	TArray<AAICharacter_B*> StoolReplacers;

	int CurrentBoxReplacerIndex = 0;
	TArray<AAICharacter_B*> BoxReplacers;

	// ********** Misc. **********

	UPROPERTY()
		UGameInstance_B* GameInstance = nullptr;
};



