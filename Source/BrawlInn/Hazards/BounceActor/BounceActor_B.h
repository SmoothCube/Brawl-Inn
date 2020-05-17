// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Throwable_B.h"
#include "BounceActor_B.generated.h"

class UDataTable_B;
class APlayerController_B;

UCLASS()
class BRAWLINN_API ABounceActor_B : public AThrowable_B
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABounceActor_B();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnPickupCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                            const FHitResult& SweepResult);

	virtual void OnItemFracture() override;

	void SpawnPlayerCharacter();

	void SetupBarrel(APlayerController_B* Controller, bool bBarrelShouldExplode = true);
	void BreakBarrel();

	UPROPERTY(EditAnywhere)
	float ExplodeTime = 5.f;

	UPROPERTY(EditAnywhere)
	float Falloff = 0.5f;

	UPROPERTY(EditAnywhere)
	float InnerRadius = 100.f;

	UPROPERTY(EditAnywhere)
	float Radius = 500.f;

	FTimerHandle TH_ExplodeTimer;

	UPROPERTY()
	APlayerController_B* PlayerController = nullptr;

	bool bShouldDestroyTarget = false;

	bool bShouldExplode = true;

};
