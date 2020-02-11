// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundBase.h"

UGameInstance_B::UGameInstance_B()
{

}

void UGameInstance_B::Init()
{
	Super::Init();
	FPlayerInfo Info;
	Info.ID = 0;
	Info.Type = EPlayerCharacterType::YUGGO;
	AddPlayerInfo(Info);

	FPlayerInfo Info2;
	Info2.ID = 1;
	Info2.Type = EPlayerCharacterType::LEADJAW;
	AddPlayerInfo(Info2);

	FPlayerInfo Info3;
	Info3.ID = 2;
	Info3.Type = EPlayerCharacterType::SWIFTBOOT;
	AddPlayerInfo(Info3);

	//FPlayerInfo Info4;
	//Info4.ID = 3;
	//Info4.Type = EPlayerCharacterType::DAGGER;
	//AddPlayerInfo(Info4);
}

void UGameInstance_B::PlayImpactCameraShake(FVector Epicenter)
{
	UGameplayStatics::PlayWorldCameraShake(GetWorld(),ImpactCameraShake, Epicenter,0.0f, 10000.f);

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
