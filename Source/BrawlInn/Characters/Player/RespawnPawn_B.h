// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RespawnPawn_B.generated.h"

class AGameCamera_B;
class ABounceActor_B;
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
	void SetCanBreakBarrel();

	void SetInputVectorX(float X);

	void SetInputVectorY(float Y);
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UDecalComponent* Decal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USphereComponent* Sphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variables")
		float MovementSpeed = 2250.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variables")
		float TimeBeforeCanThrow = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variables")
		float TimeUntilAutoThrow = 4.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variables")
		float TimeBeforeCanBreakBarrel = 0.5f;

	UPROPERTY()
		ABounceActor_B* Barrel = nullptr;

	FVector InputVector = FVector::ZeroVector;

	FTimerHandle TH_ThrowTimer;

	bool bThrowHasStarted = false;
	
	bool bBarrelIsThrown = false;

	bool bCanBreakBarrel = false;

	UPROPERTY()
		AGameCamera_B* GameCamera = nullptr;
};
