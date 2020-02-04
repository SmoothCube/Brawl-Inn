// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInstance_B.generated.h"


class USoundBase;
/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FNumberOfPlayerControllersChanged);

UCLASS()
class BRAWLINN_API UGameInstance_B : public UGameInstance
{
    GENERATED_BODY()

public:

//    UFUNCTION(BlueprintCallable, Category = "Audio", meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "3", UnsafeDuringActorConstruction = "true", Keywords = "play"))
    //static void PlaySfxSoundAtLocation(const UObject* WorldContextObject, USoundBase* Sound, FVector Location, float VolumeMultiplier, float PitchMultiplier, float StartTime, USoundAttenuation* AttenuationSettings, USoundConcurrency* ConcurrencySettings)
    //{

    //}

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Audio")
    float MasterVolume = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Audio")
    float MusicVolume = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Audio")
    float SfxVolume = 1.f;
    UFUNCTION()
    void AddPlayerControllerID(int ID);

    UFUNCTION()
    void RemovePlayerControllerID(int ID);  
    
    UFUNCTION()
    TArray<int> GetActivePlayerControllerIDs() const;

    UFUNCTION()
    void SetActivePlayerControllerIDs(TArray<int> NewControllerIDs);
    
    FNumberOfPlayerControllersChanged OnNumberPlayerControllersChanged;

protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Players")
        TArray<int> ActivePlayerControllerIDs = { 0,1 };
};
