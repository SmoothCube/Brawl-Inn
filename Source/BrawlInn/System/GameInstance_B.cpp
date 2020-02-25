// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundBase.h"

void UGameInstance_B::Init()
{
	Super::Init();
}

void UGameInstance_B::PlayImpactCameraShake(FVector Epicenter)
{
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), ImpactCameraShake, Epicenter, 0.0f, 10000.f);
}

float UGameInstance_B::GetMasterVolume() const
{
	return MasterVolume;
}

float UGameInstance_B::GetMusicVolume() const
{
	return MusicVolume;
}

float UGameInstance_B::GetSfxVolume() const
{
	return SfxVolume;
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

void UGameInstance_B::SetPlayerInfos(TArray<FPlayerInfo> NewPlayerInfos)
{
	ActivePlayerInfos = NewPlayerInfos;
	OnPlayerInfoChanged.Broadcast();
}

bool UGameInstance_B::ShouldUseSpreadSheets() const
{
	return bShouldUseSpreadSheets;
}

bool UGameInstance_B::GameIsScoreBased() const
{
	return bGameIsScoreBased;
}
