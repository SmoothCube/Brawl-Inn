// Fill out your copyright notice in the Description page of Project Settings.

#include "MovementspeedUseable_B.h"
#include "BrawlInn.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "GameFramework/CharacterMovementComponent.h"

void AMovementspeedUseable_B::Use_Implementation()
{
	Character = Cast<APlayerCharacter_B>(GetAttachParentActor());
	if (!Character) { BWarn("Could not find player while using, something is wrong! ABORT!"); return; }

	OldMovementSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
	OldAcceleration = Character->GetCharacterMovement()->MaxAcceleration;

	Character->GetCharacterMovement()->MaxWalkSpeed = OldMovementSpeed + MovementSpeedBoost;
	Character->GetCharacterMovement()->MaxAcceleration = OldAcceleration + AccelerationBoost;

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

