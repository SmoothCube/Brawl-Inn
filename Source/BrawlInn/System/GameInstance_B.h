// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Characters/Player/PlayerInfo.h"
#include "GameInstance_B.generated.h"

class USoundClass;
class UCameraShake;
class USoundCue;
class USoundBase;
class UAudioComponent;

DECLARE_MULTICAST_DELEGATE(FPlayerInfoChanged);

UCLASS()
class BRAWLINN_API UGameInstance_B : public UGameInstance
{
	GENERATED_BODY()

protected:
	void Init() override;

	// ********** Camera **********
public:
	void PlayImpactCameraShake(FVector Epicenter);

	UFUNCTION(BlueprintCallable)
		void SetCameraSwapTransform(FTransform Transform);

	UFUNCTION(BlueprintCallable)
		FTransform GetCameraSwapTransform() const;

protected:
	FTransform CameraSwapTransform = FTransform(FRotator(-70.591858f, -0.203229f, 1.407142f), FVector(208.078644f, 902.894043f, 572.226013f));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		TSubclassOf<UCameraShake> ImpactCameraShake;

	// ********** Sound **********
	UPROPERTY()
		UAudioComponent* BirdSoundComponent;

	UPROPERTY()
		UAudioComponent* RiverSoundComponent;

	UPROPERTY()
		UAudioComponent* MainMusicComponent;

	UPROPERTY()
		UAudioComponent* AnnouncerVoiceComponent;
public:

	void StartAmbientSounds();

	void SetAndPlayMusic(USoundCue* NewMusic);

	void PlayAnnouncerLine(USoundCue* Line);

	const USoundBase* GetCurrentMusic();

	float GetMasterVolume() const;

	float GetMusicVolume() const;

	float GetSfxVolume() const;

	void SetMasterVolume(float Value);

	void SetMusicVolume(float Value);

	void SetSfxVolume(float Value);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "SoundClass")
		USoundClass* MasterSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "SoundClass")
		USoundClass* MusicSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "SoundClass")
		USoundClass* SfxSoundClass;

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

	UFUNCTION(BlueprintCallable)
	void ReturnToMainMenuB();

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

	UPROPERTY(EditDefaultsOnly, Category = "EditorOnly")
		bool bIgnoreCountdown = true;

	UPROPERTY(EditDefaultsOnly, Category = "EditorOnly")
		bool bMuteMusic = true;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "EditorOnly")
		bool bFakeScoreAtStatScreen = false;
#endif

	// ********** Misc **********
	bool bMusicInitialized = false;

};
