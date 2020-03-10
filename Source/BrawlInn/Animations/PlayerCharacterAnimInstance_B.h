// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/PlayerAnimInstance_B.h"
#include "PlayerCharacterAnimInstance_B.generated.h"

/**
 * 
 */

class APlayerCharacter_B;

UCLASS()
class BRAWLINN_API UPlayerCharacterAnimInstance_B : public UPlayerAnimInstance_B
{
	GENERATED_BODY()
	
protected:

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY()
	APlayerCharacter_B* PlayerOwner = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BreakFreeBlend = 0.f;

};
