// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Items/Throwable_B.h"
#include "BounceActor_B.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class ABarrelTargetPoint_B;
class APlayerController_B;
class UNiagaraComponent;
UCLASS()
class BRAWLINN_API ABounceActor_B : public AThrowable_B
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABounceActor_B();

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void Explode(AActor* DestroyedActor);

	UPROPERTY(EditAnywhere)
	float ExplodeTime = 5.f;

	UPROPERTY(EditAnywhere)
	float Falloff = 0.5f;

	UPROPERTY(EditAnywhere)
	float InnerRadius = 100.f;

	UPROPERTY(EditAnywhere)
	float Radius = 500.f;


	FTimerHandle TH_ExplodeTimer;
	ABarrelTargetPoint_B* Target = nullptr;

	APlayerController_B* PlayerController = nullptr;

	bool bShouldDestroyTarget = false;
};
