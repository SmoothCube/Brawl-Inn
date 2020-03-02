// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TargetPoint.h"
#include "BounceActorSpawner_B.generated.h"
class ABounceActor_B;
class ABouncePath_B;
class ABarrelTargetPoint_B;
class USoundCue;
UCLASS()
class BRAWLINN_API ABounceActorSpawner_B : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABounceActorSpawner_B();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float SpawnDelay = 5.f;


	UPROPERTY(EditAnywhere , Category = "Variables")
	TSubclassOf<ABounceActor_B> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Variables")
	TSubclassOf<ABarrelTargetPoint_B> TargetPointClass;

	FTimerHandle TH_SpawnTimer;

	UPROPERTY(EditAnywhere, Category = "Variables")
	float MinSpawnTime = 5.f;

	UPROPERTY(EditAnywhere, Category = "Variables")
	float MaxSpawnTime = 15.f;
public:	
	void SpawnBarrelOnTimer();
	ABounceActor_B* SpawnBounceActor(FVector TargetLocation);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* SpawnCue;

};
