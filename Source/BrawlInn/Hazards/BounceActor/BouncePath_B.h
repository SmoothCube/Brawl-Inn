// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BouncePath_B.generated.h"

class USplineComponent;
class ABounceActor_B;

UCLASS()
class BRAWLINN_API ABouncePath_B : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABouncePath_B();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	TArray<ABounceActor_B*> ActiveBounceActors;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	class USplineComponent* SplineCurve = nullptr;
	void AddBounceActor(ABounceActor_B* inBounceActor);
};
