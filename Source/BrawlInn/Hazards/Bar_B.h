// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "System/Structs/BarDropLocations.h"
#include "Bar_B.generated.h"

class AItem_B;
class AUseable_B;
class AIdleAICharacter_B;
class AAICharacter_B;
class UGameInstance_B;
class USoundCue;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDeliverStart, AItem_B*, AAICharacter_B*);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRAWLINN_API UBar_B : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBar_B();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ********** Tankard **********
protected:
	void GiveRandomTankard(AAICharacter_B* Waiter);
	void RandomOrder();

	UPROPERTY(EditAnywhere, Category = "Tankard")
		TSubclassOf<AUseable_B> BP_Useable;

	UPROPERTY(EditDefaultsOnly, Category = "Tankard")
		FName WaiterTag = "Waiter";

	// ********** Stool **********

	UPROPERTY(EditAnywhere, Category = "Stool")
		TSubclassOf<AItem_B> BP_Stool;

	UPROPERTY(EditDefaultsOnly, Category = "Stool")
		FName StoolReplacerTag = "StoolReplacer";

	// ********** Box **********

	UPROPERTY(EditAnywhere, Category = "Box")
		TSubclassOf<AItem_B> BP_Box;

	UPROPERTY(EditDefaultsOnly, Category = "Box")
		FName BoxReplacerTag = "BoxReplacer";


	// ********** Drink ordering **********
public:
	void StartRandomOrder(float Time = -1);

	void GetOrder(AAIDropPoint_B* DropPoint);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
		USoundCue* DrinkReadySound;

	TArray<AIdleAICharacter_B*> Customers;

	FTimerHandle TH_StartOrderTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Score", meta = (Tooltip = "This value is overridden if ShouldUseSpreadSheets is enabled"))
		float TimeUntilFirstDelivery = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Score", meta = (Tooltip = "This value is overridden if ShouldUseSpreadSheets is enabled"))
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
