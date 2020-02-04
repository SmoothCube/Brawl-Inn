// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance_B.h"
#include "System/BaseActors/Character_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/ThrowComponent_B.h"

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
	bIsPunching = Owner->PunchComponent->GetIsPunching();
	bIsHolding = Owner->HoldComponent->IsHolding();
	bIsThrowing = Owner->ThrowComponent->IsThrowing();
	if(!bIsThrowing) //if you go to throw before charge, ie tap the button and not hold it
		bIsCharging = Owner->ThrowComponent->IsCharging();
	bHasFallen = Owner->GetState() == EState::EFallen;
	Speed = Owner->GetVelocity().Size();
	Direction =  CalculateDirection(Owner->GetVelocity(), Owner->GetActorRotation());
}
