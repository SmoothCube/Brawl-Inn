// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode_B.generated.h"

class APlayerStart;
class APlayerController_B;
class APlayerCharacter_B;
class ARespawnPawn_B;
class UGameInstance_B;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSpawnCharacter, APlayerController_B*, PlayerControllerReference, bool, ShouldUseVector, FTransform, SpawnTransform);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDespawnCharacter, APlayerController_B*, PlayerControllerReference, bool, bShouldRespawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpawnCharacter_NOPARAM);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDespawnCharacter_NOPARAM);

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


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<APlayerCharacter_B> BP_PlayerCharacter;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<ARespawnPawn_B> BP_RespawnPawn;

	/// ----- Spawn Character functions/variables -----

public:

	UFUNCTION()
		virtual void UpdateViewTarget(APlayerController_B* PlayerController) {};

	/// ** Delegates ** 
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FSpawnCharacter SpawnCharacter_D;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FDespawnCharacter DespawnCharacter_D;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FSpawnCharacter_NOPARAM SpawnCharacter_NOPARAM_D;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FDespawnCharacter_NOPARAM DespawnCharacter_NOPARAM_D;

	UFUNCTION()
		void SpawnCharacter(APlayerController_B* PlayerController, bool ShouldUseVector, FTransform SpawnTransform);

	UFUNCTION()
		void DespawnCharacter(APlayerController_B* PlayerController, bool bShouldRespawn);

protected:

	UPROPERTY()
	UGameInstance_B* GameInstance = nullptr;

	void CreatePlayerControllers();
	void GetAllSpawnpointsInWorld();
	FTransform GetRandomSpawnTransform();
};
