// Fill out your copyright notice in the Description page of Project Settings.

#include "PunchUseable_B.h"
#include "BrawlInn.h"
#include "Characters/Player/PlayerCharacter_B.h"

APunchUseable_B::APunchUseable_B()
{
	Duration = -1;
}

void APunchUseable_B::Use_Implementation()
{
	BScreen("STRONK PUNCH");

	APlayerCharacter_B* Character = Cast<APlayerCharacter_B>(GetAttachParentActor());
	if (!Character) { BWarn("Could not find player while using, something is wrong! ABORT!"); return; }

	Character->StunStrength = StunStrength;

	Super::Use_Implementation();
}
