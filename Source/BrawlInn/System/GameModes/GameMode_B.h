// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Characters/Player/PlayerInfo.h"
#include "GameMode_B.generated.h"

class UPauseMenu_B;
class APlayerController_B;
class AGameCamera_B;
class APlayerStart;
class AGamePlayerController_B;
class APlayerCharacter_B;
class ARespawnPawn_B;
class UGameInstance_B;
class USoundCue;
struct FPlayerInfo;

DECLARE_MULTICAST_DELEGATE(FOnCharacterSpawn);

#if WITH_EDITOR
DECLARE_MULTICAST_DELEGATE(FOnCharacterDespawn);
#endif

UCLASS()
class BRAWLINN_API AGameMode_B : public AGameModeBase
{
	GENERATED_BODY()
public:
	AGameMode_B();
	// ********** AActor **********
protected:
	void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
		virtual void PostLevelLoad();

	// ********** Input **********

	UFUNCTION(BlueprintCallable)
		void DisableControllerInputs();

	UFUNCTION(BlueprintCallable)
		void EnableControllerInputs();

	// ********** PauseMenu **********
public:
	void PauseGame(AGamePlayerController_B* ControllerThatPaused);

	UFUNCTION(BlueprintCallable)
		void ResumeGame();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserWidgets")
		TSubclassOf<UPauseMenu_B> BP_PauseMenu;

	UPROPERTY(BlueprintReadWrite)
		UPauseMenu_B* PauseMenuWidget = nullptr;

public:

	// ********** Character Spawning **********
	UFUNCTION()
		void SpawnCharacter(FPlayerInfo PlayerInfo, bool ShouldUseVector, FTransform SpawnTransform);

	UFUNCTION()
	void RespawnCharacter(FPlayerInfo PlayerInfo);
		
	AActor* SpawnRespawnPawn(FPlayerInfo PlayerInfo);

	FOnCharacterSpawn& OnCharacterSpawn();

protected:
	FOnCharacterSpawn OnCharacterSpawn_Delegate;

	UPROPERTY(EditDefaultsOnly, Category = "Character Spawning")
		TSubclassOf<ARespawnPawn_B> BP_RespawnPawn;

	UPROPERTY(EditDefaultsOnly, Category = "Character Spawning")
		TArray<TSubclassOf<APlayerCharacter_B>> BP_PlayerCharacters;

	void CreatePlayerControllers();

public:
	TArray<APlayerController_B*> GetPlayerControllers();

protected:
	UPROPERTY()
		TArray<APlayerController_B*> PlayerControllers;

#if WITH_EDITOR
public:
	FOnCharacterDespawn& OnCharacterDespawn();
	UFUNCTION()
		void DespawnCharacter(AGamePlayerController_B* PlayerController);

protected:
	FOnCharacterDespawn OnCharacterDespawn_Delegate;
#endif
protected:

	void GetAllPlayerStartsInWorld();

	UPROPERTY()
		TArray<APlayerStart*> PlayerStarts;

	FTransform GetRandomSpawnTransform();

	FTransform GetSpawnTransform(const int PlayerControllerID);

	// ********** Camera **********
public:
	AGameCamera_B* GetGameCamera() const;

	void AddCameraFocusPoint(AActor* FocusActor);

	void RemoveCameraFocusPoint(AActor* FocusActor);

	UFUNCTION(BlueprintCallable)
		void UpdateViewTargets(ACameraActor* Camera = nullptr, float BlendTime = 0, bool LockOutgoing = false);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
		TSubclassOf<AGameCamera_B> BP_GameCamera;

	UPROPERTY()
		AGameCamera_B* GameCamera = nullptr;

	// ********** Music **********

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
		USoundCue* Music;

	// ********** Misc. **********

	UPROPERTY()
		UGameInstance_B* GameInstance = nullptr;
};
