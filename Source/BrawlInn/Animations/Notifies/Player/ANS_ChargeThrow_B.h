// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_ChargeThrow_B.generated.h"

/**
 * 
 */
class APlayerCharacter_B;
UCLASS()
class BRAWLINN_API UANS_ChargeThrow_B : public UAnimNotifyState
{
    GENERATED_BODY()

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
private:
	UPROPERTY()
	APlayerCharacter_B* Player = nullptr;
	float CurrentTime = 0.f;
	float AnimLength = 0.f;

};
