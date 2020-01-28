// Fill out your copyright notice in the Description page of Project Settings.

#include "MovementspeedUseable_B.h"
#include "BrawlInn.h"
#include "Player/PlayerCharacter_B.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

void AMovementspeedUseable_B::Use_Implementation()
{

	Character = Cast<APlayerCharacter_B>(GetAttachParentActor());
	if (!Character) { BWarn("Could not find player while using, something is wrong!"); return; }

	OldMovementSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
	OldAcceleration = Character->GetCharacterMovement()->MaxAcceleration;

	Character->GetCharacterMovement()->MaxWalkSpeed = OldMovementSpeed + MovementSpeedBoost;
	Character->GetCharacterMovement()->MaxAcceleration = OldAcceleration + AccelerationBoost;

	GetWorld()->GetTimerManager().SetTimer(TH_DrinkHandle, this, &AMovementspeedUseable_B::ResetBoost, Duration, false);

	Super::Use_Implementation();
}

void AMovementspeedUseable_B::ResetBoost()
{
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = OldMovementSpeed;
		Character->GetCharacterMovement()->MaxAcceleration = OldAcceleration;
	}
}

void AMovementspeedUseable_B::FellOutOfWorld(const UDamageType& dmgType)
{
	if (!GetWorld()->GetTimerManager().TimerExists(TH_DrinkHandle))
	{
		Super::FellOutOfWorld(dmgType);
	}
}
