// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance_B.h"

#include "ConfigCacheIni.h"
#include "DataTable_B.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundBase.h"

void UGameInstance_B::Init()
{
	Super::Init();

	UDataTable_B* DataTable = NewObject<UDataTable_B>();
	DataTable->LoadCSVFile(FStringValues::StaticStruct(), "MapNames.csv");
	GameMap = DataTable->GetRow<FStringValues>("GameMap")->Value;
	MenuMap = DataTable->GetRow<FStringValues>("MenuMap")->Value;
	MenuMapToStream = DataTable->GetRow<FStringValues>("MenuMapToStream")->Value;
	DataTable->ConditionalBeginDestroy();

	
	GConfig->GetFloat(TEXT("BrawlInn.Audio"), TEXT("Master Volume"), MasterVolume, GGameIni);
	GConfig->GetFloat(TEXT("BrawlInn.Audio"), TEXT("Music Volume"), MusicVolume, GGameIni);
	GConfig->GetFloat(TEXT("BrawlInn.Audio"), TEXT("Sfx Volume"), SfxVolume, GGameIni);
	
}

void UGameInstance_B::PlayImpactCameraShake(FVector Epicenter)
{
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), ImpactCameraShake, Epicenter, 0.0f, 10000.f);
}

void UGameInstance_B::SetCameraSwapTransform(const FTransform Transform)
{
	CameraSwapTransform = Transform;
}

FTransform UGameInstance_B::GetCameraSwapTransform() const
{
	return CameraSwapTransform;
}

float UGameInstance_B::GetMasterVolume() const
{
	return MasterVolume;
}

float UGameInstance_B::GetMusicVolume() const
{
#if WITH_EDITOR
	if (bMuteMusic)
		return 0;
#endif
	return MusicVolume;
}

float UGameInstance_B::GetSfxVolume() const
{
	return SfxVolume;
}

void UGameInstance_B::SetMasterVolume(const float Value)
{
	MasterVolume = Value;
}

void UGameInstance_B::SetMusicVolume(const float Value)
{
	MusicVolume = Value;
}

void UGameInstance_B::SetSfxVolume(const float Value)
{
	SfxVolume = Value;
}

void UGameInstance_B::AddPlayerInfo(FPlayerInfo PlayerInfo)
{
	if (!ActivePlayerInfos.FindByPredicate([&](const FPlayerInfo& Info) {return Info.ID == PlayerInfo.ID; }))
	{
		ActivePlayerInfos.Add(PlayerInfo);
		OnPlayerInfoChanged.Broadcast();
	}
}

void UGameInstance_B::RemovePlayerInfo(int ID)
{
	ActivePlayerInfos.RemoveAll([&](const FPlayerInfo& PlayerInfo) {
		return PlayerInfo.ID == ID;
		});
	OnPlayerInfoChanged.Broadcast();
}

TArray<FPlayerInfo> UGameInstance_B::GetPlayerInfos() const
{
	return ActivePlayerInfos;
}

FPlayerInfo UGameInstance_B::GetPlayerInfo(const int ID) const
{
	const auto t = ActivePlayerInfos.FindByPredicate([&](const FPlayerInfo& PlayerInfo) {
		return PlayerInfo.ID == ID;
		});
	if (t)
		return *t;

	return FPlayerInfo();
}

void UGameInstance_B::SetPlayerInfos(TArray<FPlayerInfo> NewPlayerInfos)
{
	ActivePlayerInfos = NewPlayerInfos;
	OnPlayerInfoChanged.Broadcast();
}

bool UGameInstance_B::ShouldUseSpreadSheets() const
{
	return bShouldUseSpreadSheets;
}

bool UGameInstance_B::IgnoreCountdown() const
{
#if WITH_EDITOR
	return bIgnoreCountdown;
#else
	return false;
#endif
}
