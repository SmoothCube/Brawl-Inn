// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Characters/Player/PlayerInfo.h"
#include "GameMode_B.generated.h"

class APlayerStart;
class APlayerController_B;
class APlayerCharacter_B;
class ARespawnPawn_B;
class UGameInstance_B;
class USoundCue;
struct FPlayerInfo;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FSpawnCharacter, FPlayerInfo, bool, FTransform);
DECLARE_MULTICAST_DELEGATE_OneParam(FDespawnCharacter, APlayerController_B*);
DECLARE_MULTICAST_DELEGATE_OneParam(FRespawnCharacter, FPlayerInfo);
DECLARE_MULTICAST_DELEGATE(FSpawnCharacter_NOPARAM);
DECLARE_MULTICAST_DELEGATE(FDespawnCharacter_NOPARAM);
DECLARE_MULTICAST_DELEGATE(FOnRespawnCharacter);


UCLASS()
class BRAWLINN_API AGameMode_B : public AGameModeBase
{
	GENERATED_BODY()
public:

	virtual void BeginPlay() override;

	TArray<APlayerController*> PlayerControllers;
protected:

	UPROPERTY(BlueprintReadWrite)
		TArray<APlayerStart*> Spawnpoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		TArray<TSubclassOf<APlayerCharacter_B>> BP_PlayerCharacters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		TSubclassOf<ARespawnPawn_B> BP_RespawnPawn;

	/// ----- Spawn Character functions/variables -----

public:

	UFUNCTION()
		virtual void UpdateViewTarget(APlayerController_B* PlayerController) {};

	/// ** Delegates ** 
		FSpawnCharacter SpawnCharacter_D;

		FDespawnCharacter DespawnCharacter_D;

		FRespawnCharacter RespawnCharacter_D;
		
		FSpawnCharacter_NOPARAM SpawnCharacter_NOPARAM_D;

		FDespawnCharacter_NOPARAM DespawnCharacter_NOPARAM_D;

		FOnRespawnCharacter OnRespawnCharacter_D;

	UFUNCTION()
		void SpawnCharacter(FPlayerInfo PlayerInfo, bool ShouldUseVector, FTransform SpawnTransform);

	UFUNCTION()
		void DespawnCharacter(APlayerController_B* PlayerController);

	UFUNCTION()
		void RespawnCharacter(FPlayerInfo PlayerInfo);

protected:

	UPROPERTY()
	UGameInstance_B* GameInstance = nullptr;

	void CreatePlayerControllers();
	void GetAllSpawnpointsInWorld();
	FTransform GetRandomSpawnTransform();

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* Music;
};
