// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Characters/Player/PlayerInfo.h"
#include "GameInstance_B.generated.h"

class UCameraShake;

DECLARE_MULTICAST_DELEGATE(FPlayerInfoChanged);

UCLASS()
class BRAWLINN_API UGameInstance_B : public UGameInstance
{
	GENERATED_BODY()

	// ********** UGameInstance **********
protected:
	virtual void Init() override;

	// ********** Camera **********
public:
	void PlayImpactCameraShake(FVector Epicenter);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Audio")
		TSubclassOf<UCameraShake> ImpactCameraShake;

	// ********** Sound **********
public:
	float GetMasterVolume() const;

	float GetMusicVolume() const;
	
	float GetSfxVolume() const;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Audio")
		float MasterVolume = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Audio")
		float MusicVolume = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Audio")
		float SfxVolume = 1.f;

	// ********** PlayerInfo **********
public:
	void AddPlayerInfo(FPlayerInfo PlayerInfo);

	void RemovePlayerInfo(int ID);

	TArray<FPlayerInfo> GetPlayerInfos() const;

	FPlayerInfo GetPlayerInfo(int ID) const;

	void SetPlayerInfos(TArray<FPlayerInfo> NewPlayerInfos);

	FPlayerInfoChanged OnPlayerInfoChanged;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Players")
		TArray<FPlayerInfo> ActivePlayerInfos;

	// ********** Settings **********

public:

	bool ShouldUseSpreadSheets() const;
	bool GameIsScoreBased() const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variables|Settings")
	bool bShouldUseSpreadSheets = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variables|Settings")
		bool bGameIsScoreBased = true;
};
