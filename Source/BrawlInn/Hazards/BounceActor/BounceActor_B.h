// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Items/Throwable_B.h"
#include "BounceActor_B.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class BRAWLINN_API ABounceActor_B : public AThrowable_B
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABounceActor_B();

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
