// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance_B.h"
#include "Characters/Character_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "BrawlInn.h"

void UPlayerAnimInstance_B::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Owner = Cast<ACharacter_B>(TryGetPawnOwner());
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

	if (Owner->HoldComponent)
		bIsHolding = Owner->HoldComponent->IsHolding();
	if (Owner->ThrowComponent)
	{
		bIsThrowing = Owner->ThrowComponent->IsThrowing();
		if(!bIsThrowing) //if you go to throw before charge, ie tap the button and not hold it
			bIsChargingThrow = Owner->IsCharging();
	}
	bHasFallen = Owner->GetState() == EState::EFallen;
	Speed = Owner->GetVelocity().Size();
	Direction =  CalculateDirection(Owner->GetVelocity(), Owner->GetActorRotation());
}
