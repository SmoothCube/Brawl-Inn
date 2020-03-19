// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterAnimInstance_B.h"

#include "BrawlInn.h"
#include "Characters/Player/PlayerCharacter_B.h"


void UPlayerCharacterAnimInstance_B::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	PlayerOwner = Cast<APlayerCharacter_B>(Owner);
}	

void UPlayerCharacterAnimInstance_B::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (!PlayerOwner) { return; }

	BreakFreeBlend = PlayerOwner->BreakFreeAnimationBlend;

	if (bIsThrowing)
		BWarn("%s is throwing!", *GetNameSafe(PlayerOwner));
	if (bIsChargingThrow)
		BWarn("%s is charging throw!", *GetNameSafe(PlayerOwner));
}

