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

DECLARE_MULTICAST_DELEGATE_ThreeParams(FSpawnCharacter, FPlayerInfo, bool, FTransform);
DECLARE_MULTICAST_DELEGATE_OneParam(FRespawnCharacter, FPlayerInfo);
DECLARE_MULTICAST_DELEGATE(FSpawnCharacter_NOPARAM);
DECLARE_MULTICAST_DELEGATE(FOnRespawnCharacter);

#if WITH_EDITOR
DECLARE_MULTICAST_DELEGATE(FDespawnCharacter_NOPARAM);
DECLARE_MULTICAST_DELEGATE_OneParam(FDespawnCharacter, AGamePlayerController_B*);
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

public:
	UFUNCTION(BlueprintCallable)
		virtual void PostLevelLoad();

	UFUNCTION(BlueprintCallable)

		void DisableControllerInputs();

	UFUNCTION(BlueprintCallable)
		void EnableControllerInputs();

	// ********** PauseMenu **********

	void PauseGame(AGamePlayerController_B* ControllerThatPaused);

	UFUNCTION(BlueprintCallable)
		void ResumeGame();

	UPROPERTY(BlueprintReadWrite)
		UPauseMenu_B* PauseMenuWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UserWidgets")
		TSubclassOf<UPauseMenu_B> BP_PauseMenu;

	// ********** Misc. **********

	UPROPERTY()
		TArray<APlayerController_B*> PlayerControllers;
protected:

	UPROPERTY(BlueprintReadWrite)
		TArray<APlayerStart*> Spawnpoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSubclassOf<APlayerCharacter_B>> BP_PlayerCharacters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<ARespawnPawn_B> BP_RespawnPawn;

	/// ----- Spawn Character functions/variables -----

public:

	UFUNCTION(BlueprintCallable)
		void UpdateViewTargets(ACameraActor* Camera = nullptr, float BlendTime = 0, bool LockOutgoing = false);

	/// ** Delegates ** 
	FSpawnCharacter SpawnCharacter_D;
	FSpawnCharacter_NOPARAM SpawnCharacter_NOPARAM_D;

	FRespawnCharacter RespawnCharacter_D;
	FOnRespawnCharacter OnRespawnCharacter_D;

#if WITH_EDITOR
	FDespawnCharacter DespawnCharacter_D;
	FDespawnCharacter_NOPARAM DespawnCharacter_NOPARAM_D;
	UFUNCTION()
		void DespawnCharacter(AGamePlayerController_B* PlayerController);
#endif


	UFUNCTION()
		void SpawnCharacter(FPlayerInfo PlayerInfo, bool ShouldUseVector, FTransform SpawnTransform);


	UFUNCTION()
		void RespawnCharacter(FPlayerInfo PlayerInfo);

	AGameCamera_B* GetGameCamera() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AGameCamera_B> BP_GameCamera;

	void AddCameraFocusPoint(AActor* FocusActor);
	void RemoveCameraFocusPoint(AActor* FocusActor);


	UPROPERTY()
		AGameCamera_B* GameCamera = nullptr;
protected:

	UPROPERTY()
		UGameInstance_B* GameInstance = nullptr;

	void CreatePlayerControllers();
	void GetAllSpawnpointsInWorld();
	FTransform GetRandomSpawnTransform();
	FTransform GetSpawnTransform(const int PlayerControllerID);

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* Music;
};
