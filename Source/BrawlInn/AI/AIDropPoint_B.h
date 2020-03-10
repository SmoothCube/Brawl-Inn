// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "System/Structs/BarDropLocations.h"
#include "AIDropPoint_B.generated.h"

class AItem_B;
class ABar_B;

UCLASS()
class BRAWLINN_API AAIDropPoint_B : public ATargetPoint
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	void ItemDestroyed();

	UPROPERTY(EditAnywhere)
		TSubclassOf<AItem_B> ItemToSpawn;

	UPROPERTY(EditAnywhere)
		EBarDropLocationType Type = EBarDropLocationType::Stool;

	UPROPERTY(BlueprintReadOnly)
	ABar_B* Bar = nullptr;

	UPROPERTY()
		AItem_B* Item = nullptr;

public:
	UFUNCTION()
		void SetNewItem(AItem_B* NewItem);
};
