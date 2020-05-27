// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance_B.h"
#include "Characters/Character_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "Items/Useable_B.h"
#include "BrawlInn.h"

void UPlayerAnimInstance_B::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Owner = Cast<ACharacter_B>(TryGetPawnOwner());
	RandomIdleStartTime = FMath::FRandRange(0.f, 1.5f);
}

bool UPlayerAnimInstance_B::IsReady() const
{
	return (IsValid(Owner));
}

void UPlayerAnimInstance_B::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!IsReady())
		return;
	if (Owner->PunchComponent)
	{
		bIsPunching = Owner->PunchComponent->GetIsPunching();
		bIsChargingPunch = Owner->IsCharging();
	}
	bIsBeingHeld = (Owner->GetState() == EState::EBeingHeld);
	bIsHoldingUseable = false;
	if (Owner->HoldComponent)
	{
		bIsHolding = Owner->HoldComponent->IsHolding();
		if (bIsHolding)
		{
			if (Owner->HoldComponent->GetHoldingItem()->IsA(AUseable_B::StaticClass()))
			{
				bIsHoldingUseable = true;
			}
		}
	}
	if (Owner->ThrowComponent)
	{
		bIsDrinking = Owner->ThrowComponent->IsDrinking();

		bIsFinishedDrinking = Owner->ThrowComponent->IsDrinkingFinished();

		bIsThrowing = Owner->ThrowComponent->IsThrowing();
		if(!bIsThrowing) //if you go to throw before charge, ie tap the button and not hold it
			bIsChargingThrow = Owner->IsCharging();
	}
	bHasFallen = Owner->GetState() == EState::EFallen;
	if (bHasFallen)
		bIsFacingUp = Owner->IsFacingUp();
	Speed = Owner->GetVelocity().Size();
	Direction =  CalculateDirection(Owner->GetVelocity(), Owner->GetActorRotation());
}
