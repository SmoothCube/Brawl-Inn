// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldUseable_B.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "BrawlInn.h"

void AShieldUseable_B::Use_Implementation()
{
	Character = Cast<APlayerCharacter_B>(GetAttachParentActor());
	if (!Character) { BWarn("Could not find player while using, something is wrong! ABORT!"); return; }

	Character->ApplyShield();

	Super::Use_Implementation();
}

void AShieldUseable_B::ResetBoost()
{
	Character->RemoveShield();
}