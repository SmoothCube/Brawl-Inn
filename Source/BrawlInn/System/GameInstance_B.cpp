// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundBase.h"

UGameInstance_B::UGameInstance_B()
{
	FPlayerInfo Info;
	Info.ID = 0;
//	Info.Type = EPlayerCharacterType::YUGGO;
	ActivePlayerInfos.Add(Info);

	FPlayerInfo Info2;
	Info2.ID = 1;
	//Info2.Type = EPlayerCharacterType::SWIFTBOOT;
	ActivePlayerInfos.Add(Info2);
}

void UGameInstance_B::AddPlayerInfo(FPlayerInfo PlayerInfo)
{
	ActivePlayerInfos.Add(PlayerInfo);
	OnPlayerInfoChanged.Broadcast();
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
