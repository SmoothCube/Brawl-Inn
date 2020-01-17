// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode_B.generated.h"

class APlayerStart;
class APlayerController_B;
class APlayerCharacter_B;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnCharacter, APlayerController_B*, PlayerControllerReference);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDespawnCharacter, APlayerController_B*, PlayerControllerReference);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpawnCharacter_NOPARAM);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDespawnCharacter_NOPARAM);




UCLASS()
class BRAWLINN_API AGameMode_B : public AGameModeBase
{
	GENERATED_BODY()
public:

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FSpawnCharacter SpawnCharacter_D;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDespawnCharacter DespawnCharacter_D;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FSpawnCharacter_NOPARAM SpawnCharacter_NOPARAM_D;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FDespawnCharacter_NOPARAM DespawnCharacter_NOPARAM_D;

protected:

	UPROPERTY(BlueprintReadWrite)
	TArray<APlayerStart*> Spawnpoints;

	TArray<APlayerController*> PlayerControllers;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APlayerCharacter_B> BP_PlayerCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int NumberOfPlayers = 4;


	UFUNCTION()
	void SpawnCharacter(APlayerController_B* PlayerController);

	UFUNCTION()
	void DespawnCharacter(APlayerController_B* PlayerController);

	FTransform GetRandomSpawnTransform();
};
