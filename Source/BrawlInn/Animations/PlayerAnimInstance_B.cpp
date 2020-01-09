// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance_B.h"
#include "Player/PlayerCharacter_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/HoldComponent_B.h"

void UPlayerAnimInstance_B::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Owner = Cast<APlayerCharacter_B>(TryGetPawnOwner());
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
	Speed = Owner->GetVelocity().Size();
	Direction =  CalculateDirection(Owner->GetVelocity(), Owner->GetActorRotation());
}
