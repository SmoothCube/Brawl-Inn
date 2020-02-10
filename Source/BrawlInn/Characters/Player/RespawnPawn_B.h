// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RespawnPawn_B.generated.h"

class ABounceActor_B;
UCLASS()
class BRAWLINN_API ARespawnPawn_B : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARespawnPawn_B();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void PossessedBy(AController* NewController) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ThrowBarrel();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UDecalComponent* Decal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeUntilThrow = 10.f;

	FVector InputVector = FVector::ZeroVector;

	FTimerHandle TH_ThrowTimer;

	bool bBarrelIsThrown = false;

	bool bHasRespawned = false;

	ABounceActor_B* Barrel = nullptr;
};
