// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_FleeFromPlayer_B.generated.h"

class AAICharacter_B;
/**
 *
 */
UCLASS()
class BRAWLINN_API UBTS_FleeFromPlayer_B : public UBTService
{
	GENERATED_BODY()
protected:
	UBTS_FleeFromPlayer_B();

	void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
		void OnHearNoise(APawn* Instigator, const FVector& Location, float Volume);

	float NormalMaxAcceleration;
	float NormalMaxWalkSpeed;

	UPROPERTY(EditAnywhere)
		float FleeMaxAcceleration = 3000;

	UPROPERTY(EditAnywhere)
		float FleeMaxWalkSpeed = 2000;

	UPROPERTY(EditAnywhere)
		float ResetSpeedTime = 0.5f;

	UPROPERTY()
		AAICharacter_B* Character = nullptr;

	FTimerHandle TH_ResetSpeed;


};
