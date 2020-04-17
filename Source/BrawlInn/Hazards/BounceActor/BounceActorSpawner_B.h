// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BounceActorSpawner_B.generated.h"

class ABounceActor_B;
class USoundCue;
class UStaticMeshComponent;

UCLASS()
class BRAWLINN_API ABounceActorSpawner_B : public AActor
{
	GENERATED_BODY()

		ABounceActorSpawner_B();
protected:
	// ********** Components **********
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* MainMesh;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* SmallCogMesh;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* BigCogMesh;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* CannonBarrelMesh;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* BarrelSpawnLocation;
public:

	// ********** Overrides **********
	virtual void Tick(float DeltaTime) override;

	// ********** Barrel Spawning **********
	ABounceActor_B* SpawnBounceActor(FVector TargetLocation);
protected:

	UPROPERTY(EditAnywhere)
		TSubclassOf<ABounceActor_B> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* SpawnCue;

	// ********** Cannon Rotation **********
public:
	void AddRotateTarget(AActor* NewTarget);
	void RemoveRotateTarget(AActor* NewTarget);
protected:
	TArray<AActor*> RotateTargets;

	float CannonRotateSpeed = 180.f;
	// ********** Cannon Barrel Rotation **********
	UPROPERTY(EditAnywhere, Category = "Barrel Rotation")
	float HighestBarrelRotation = 15.f;
	
	UPROPERTY(EditAnywhere, Category = "Barrel Rotation")
	float LowestBarrelRotation = 65.f;
	// ********** Misc **********

	float CogRotateSpeed = 50.f;
};
