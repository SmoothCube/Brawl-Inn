// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstance_B.generated.h"

DECLARE_MULTICAST_DELEGATE(FNumberOfPlayerControllersChanged);

UCLASS()
class BRAWLINN_API UGameInstance_B : public UGameInstance
{
	GENERATED_BODY()

public:

    UFUNCTION()
    void AddPlayerControllerID(int ID);

    UFUNCTION()
    void RemovePlayerControllerID(int ID);  
    
    TArray<int> GetActivePlayerControllerIDs() const;

    void SetActivePlayerControllerIDs(TArray<int> NewControllerIDs);

    FNumberOfPlayerControllersChanged OnNumberPlayerControllersChanged;

protected:

    UPROPERTY(EditDefaultsOnly)
        TArray<int> ActivePlayerControllerIDs = { 0,1 };
};
