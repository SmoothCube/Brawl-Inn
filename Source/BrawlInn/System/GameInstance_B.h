// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Characters/Player/PlayerInfo.h"
#include "GameInstance_B.generated.h"

class UCameraShake;
class USoundCue;
class USoundBase;
class UAudioComponent;

DECLARE_MULTICAST_DELEGATE(FPlayerInfoChanged);

UCLASS()
class BRAWLINN_API UGameInstance_B : public UGameInstance
{
	GENERATED_BODY()

public:

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		TSubclassOf<UCameraShake> ImpactCameraShake;

	// ********** Sound **********
	UPROPERTY()
	UAudioComponent* BirdSoundComponent;

	UPROPERTY()
	UAudioComponent* RiverSoundComponent;
	
	UPROPERTY()
	UAudioComponent* MainMusicComponent;
public:

	void StartAmbientSounds();

	void SetAndPlayMusic(USoundCue* NewMusic);

	const USoundBase* GetCurrentMusic();

	float GetMasterVolume() const;

	float GetMusicVolume() const;

	float GetSfxVolume() const;

	void SetMasterVolume(float Value);

	void SetMusicVolume(float Value);

	void SetSfxVolume(float Value);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		float MasterVolume = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		float MusicVolume = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		float SfxVolume = 1.f;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* BirdsCue;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* RiverCue;
	// ********** PlayerInfo **********
public:
	void AddPlayerInfo(FPlayerInfo PlayerInfo);

	void RemovePlayerInfo(int ID);

	TArray<FPlayerInfo> GetPlayerInfos() const;

	FPlayerInfo GetPlayerInfo(int ID) const;

	void SetPlayerInfos(TArray<FPlayerInfo> NewPlayerInfos);

	FPlayerInfoChanged OnPlayerInfoChanged;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Players")
		TArray<FPlayerInfo> ActivePlayerInfos;

	// ********** Settings **********

public:

	bool ShouldUseSpreadSheets() const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
		bool bShouldUseSpreadSheets = false;

	// ********** Maps **********

public:

	const FString& GetGameMapName() const;
	
	const FString& GetMenuMapName() const;
	
	const FString& GetVictoryMapName() const;
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Maps")
		FString GameMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Maps")
		FString MenuMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Maps")
		FString VictoryMap;

	// ********** Editor Only Settings **********
public:
	bool IgnoreCountdown() const;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|EditorOnly")
		bool bIgnoreCountdown = true;

	UPROPERTY(EditDefaultsOnly, Category = "Settings|EditorOnly")
		bool bMuteMusic = true;

	// ********** Misc **********
	bool bMusicInitialized = false;

};
