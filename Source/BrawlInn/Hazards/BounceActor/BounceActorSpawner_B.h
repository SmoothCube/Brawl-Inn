// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BounceActorSpawner_B.generated.h"

class ABounceActor_B;
class ARespawnPawn_B;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* BarrelMidMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	void RotateBarrel(float DeltaTime, FVector TargetLocation);

	void RotateMainCannon(float DeltaTime, FVector TargetLocation);

	void RotateCogs(float DeltaTime);

	// ********** Barrel Spawning **********
	ABounceActor_B* SpawnBounceActor();
protected:

	UPROPERTY(EditAnywhere)
		TSubclassOf<ABounceActor_B> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* SpawnCue;

	// ********** Cannon Rotation **********
public:
	void AddRotateTarget(AActor* NewTarget);
	void RemoveRotateTarget(AActor* NewTarget);

	void AddShootTarget(ARespawnPawn_B* NewTarget);
protected:
	UPROPERTY()
	TArray<AActor*> RotateTargets;
	
	UPROPERTY()
	TArray<ARespawnPawn_B*> ShootTargets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CannonRotateSpeed = 18.f;

	// ********** Barrel Rotation **********
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LowestBarrelPitch = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HighestBarrelPitch = 75.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BarrelRotationSpeed = 50.f;

	// ********** Misc **********

	UPROPERTY(EditAnywhere, Category = "Cog Rotation")
	float CogRotateSpeed = 50.f;

	float CurrentCogPitch = 0.f;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void PlayAnimationMontages();

//	bool IsShooting();
//	void SetIsShooting(bool Value);

	bool IsAnimationFinished();
	void SetAnimationFinished(bool Value);

	void SetAnimationFinishedTimer();
protected:

//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	bool bIsShooting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAnimationFinished = true;

	FTimerHandle TH_ResetIsShooting;

};
