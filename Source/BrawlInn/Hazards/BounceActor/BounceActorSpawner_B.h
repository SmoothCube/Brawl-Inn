// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BounceActorSpawner_B.generated.h"

class ABounceActor_B;
class USoundCue;

UCLASS()
class BRAWLINN_API ABounceActorSpawner_B : public AActor
{
	GENERATED_BODY()

		ABounceActorSpawner_B();

public:
	// ********** Barrel Spawning **********
	ABounceActor_B* SpawnBounceActor(FVector TargetLocation);
protected:

	UPROPERTY(EditAnywhere, Category = "Variables")
		TSubclassOf<ABounceActor_B> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Variables|Audio")
		USoundCue* SpawnCue;
};
