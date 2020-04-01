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

	UFUNCTION(BlueprintCallable)
		void SetCameraSwapTransform(FTransform Transform);

	UFUNCTION(BlueprintCallable)
		FTransform GetCameraSwapTransform() const;

protected:
	FTransform CameraSwapTransform = FTransform(FRotator(-35.667469f, 5.636588f, -0.000007f), FVector(-302.956f, 880.419f, 657.871f));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Audio")
		TSubclassOf<UCameraShake> ImpactCameraShake;

	// ********** Sound **********
public:
	float GetMasterVolume() const;

	float GetMusicVolume() const;

	float GetSfxVolume() const;

	void SetMasterVolume(float Value);

	void SetMusicVolume(float Value);

	void SetSfxVolume(float Value);

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
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variables|Settings")
		bool bShouldUseSpreadSheets = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variables|Settings")
		FString GameMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variables|Settings")
		FString MenuMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variables|Settings")
		FString MenuMapToStream;

	// ********** Editor Only Settings **********
public:
	bool IgnoreCountdown() const;

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Settings|EditorOnly")
		bool bIgnoreCountdown = true;

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Settings|EditorOnly")
		bool bMuteMusic = true;

};
