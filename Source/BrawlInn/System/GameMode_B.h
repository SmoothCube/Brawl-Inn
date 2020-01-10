// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode_B.generated.h"

class APlayerStart;
class APlayerController_B;
class APlayerCharacter_B;
class AGameCamera_B;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnCharacter, APlayerController_B*, PlayerControllerReference);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDespawnCharacter, APlayerController_B*, PlayerControllerReference);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameModeInit);



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
	FGameModeInit GameModeInit_D;

protected:

	UPROPERTY(BlueprintReadWrite)
	TArray<APlayerStart*> Spawnpoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGameCamera_B> BP_GameCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APlayerCharacter_B> BP_PlayerCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int NumberOfPlayers = 4;
private:

	AGameCamera_B* GameCamera = nullptr;

	UFUNCTION()
	void SpawnCharacter(APlayerController_B* PlayerController);

	UFUNCTION()
	void DespawnCharacter(APlayerController_B* PlayerController);

	FTransform GetRandomSpawnTransform();
};
