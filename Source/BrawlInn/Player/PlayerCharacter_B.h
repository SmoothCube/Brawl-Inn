// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter_B.generated.h"

UCLASS()
class BRAWLINN_API APlayerCharacter_B : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter_B();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector InputVector = FVector::ZeroVector;
	FVector RotationVector = FVector::ZeroVector;

protected:
	UPROPERTY(EditAnywhere)
	float MaxSpeed = 1000.f;

};
