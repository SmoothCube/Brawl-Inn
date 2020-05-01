// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RespawnPawn_B.generated.h"

class AGameCamera_B;
class ABounceActor_B;
class ABounceActorSpawner_B;
class USphereComponent;

UCLASS()
class BRAWLINN_API ARespawnPawn_B : public APawn
{
	GENERATED_BODY()

		ARespawnPawn_B();

	// ********** AActor **********
protected:
	virtual void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PossessedBy(AController* NewController) override;
	void StartThrow();

	// ********** Spawning **********
public:

	void ThrowBarrel();

	void SetupBarrel(ABounceActor_B* NewBarrel); // can probably find a better name for this

	void SetCanBreakBarrel();

	void SetInputVectorX(float X);

	void SetInputVectorY(float Y);
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UDecalComponent* Decal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USphereComponent* Sphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float MovementSpeed = 2250.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float TimeBeforeCanThrow = 0.5f;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float TimeUntilAutoThrow = 4.f;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float TimeBeforeCanBreakBarrel = 0.5f;

	UPROPERTY()
		ABounceActor_B* Barrel = nullptr;

	UPROPERTY()
		ABounceActorSpawner_B* Cannon;

	FVector InputVector = FVector::ZeroVector;

	FTimerHandle TH_ThrowTimer;

	bool bThrowHasStarted = false;
	
	bool bBarrelIsThrown = false;

	bool bCanBreakBarrel = false;

	UPROPERTY()
		AGameCamera_B* GameCamera = nullptr;
};
