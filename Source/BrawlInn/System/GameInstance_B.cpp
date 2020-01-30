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