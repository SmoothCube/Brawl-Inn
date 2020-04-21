// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BounceActorSpawner_B.generated.h"

class ABounceActor_B;
class UAudioComponent;
class USoundCue;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class UMergeMeshComponent_B;

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
		USkeletalMeshComponent* BarrelLowMesh;

	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* BarrelMidMesh;

	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* BarrelTopMesh;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* BarrelSpawnLocation;

	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* OperatorNPCMesh;

	UPROPERTY(VisibleAnywhere)
		UMergeMeshComponent_B* MergeMeshComponent;
	
	UPROPERTY(VisibleAnywhere)
		UAudioComponent* EngineSoundComponent;

	UPROPERTY(VisibleAnywhere)
		UAudioComponent* CogSoundComponent;

public:

	// ********** Overrides **********

	virtual void BeginPlay() override;

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

	// ********** Misc **********

	UPROPERTY(EditAnywhere, Category = "Cog Rotation")
	float CogRotateSpeed = 50.f;

	float CurrentCogPitch = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsShooting = false;

};
