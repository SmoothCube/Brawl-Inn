// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode_B.generated.h"

class APlayerStart;
class APlayerController_B;
class APlayerCharacter_B;
class ACameraActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnCharacter, APlayerController_B*, PlayerControllerReference);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDespawnCharacter, APlayerController_B*, PlayerControllerReference);


UCLASS()
class BRAWLINN_API AGameMode_B : public AGameModeBase
{
	GENERATED_BODY()
public:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
	FSpawnCharacter SpawnCharacter_D;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
	FDespawnCharacter DespawnCharacter_D;

protected:

	UPROPERTY(BlueprintReadWrite)
	TArray<APlayerStart*> Spawnpoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ACameraActor* GameCamera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APlayerCharacter_B> PlayerToSpawn;


private:
	unsigned int NumberOfPlayers = 4;

	UFUNCTION()
	void SpawnCharacter(APlayerController_B* PlayerController);

	UFUNCTION()
	void DespawnCharacter(APlayerController_B* PlayerController);

	FTransform GetRandomSpawnTransform();
};
