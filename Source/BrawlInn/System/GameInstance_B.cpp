// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundBase.h"

//void UGameInstance_B::PlaySfxSoundAtLocation(const UObject* WorldContextObject, USoundBase* Sound, FVector Location, float VolumeMultiplier, float PitchMultiplier, float StartTime, USoundAttenuation* AttenuationSettings, USoundConcurrency* ConcurrencySettings)
//{
//	UGameplayStatics::PlaySoundAtLocation(
//		WorldContextObject,
//		Sound,
//		Location,
//		VolumeMultiplier * MasterVolume * SfxVolume,
//		PitchMultiplier,
//		StartTime,
//		AttenuationSettings,
//		ConcurrencySettings);
//}

//static void UGameInstance_B::PlaySfxSoundAtLocation(const UObject* WorldContextObject, USoundBase* Sound, FVector Location, float VolumeMultiplier, float PitchMultiplier, float StartTime, USoundAttenuation* AttenuationSettings, USoundConcurrency* ConcurrencySettings)
//{
//    UGameplayStatics::PlaySoundAtLocation(WorldContextObject, Sound, Location, VolumeMultiplier * MasterVolume * SfxVolume, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings);
//}
void UGameInstance_B::AddPlayerControllerID(int ID)
{
	ActivePlayerControllerIDs.Add(ID);
	if (OnNumberPlayerControllersChanged.IsBound())
		OnNumberPlayerControllersChanged.Broadcast();
}

void UGameInstance_B::RemovePlayerControllerID(int ID)
{
	ActivePlayerControllerIDs.Remove(ID);
	if (OnNumberPlayerControllersChanged.IsBound())
		OnNumberPlayerControllersChanged.Broadcast();
}

TArray<int> UGameInstance_B::GetActivePlayerControllerIDs() const
{
	return ActivePlayerControllerIDs;
}

void UGameInstance_B::SetActivePlayerControllerIDs(TArray<int> NewControllerIDs)
{
	ActivePlayerControllerIDs = NewControllerIDs;
	if (OnNumberPlayerControllersChanged.IsBound())
		OnNumberPlayerControllersChanged.Broadcast();
}
