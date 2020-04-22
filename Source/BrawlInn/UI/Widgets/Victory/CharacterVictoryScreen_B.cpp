// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterVictoryScreen_B.h"

#include "StatEntry_B.h"
#include "TextBlock.h"

void UCharacterVictoryScreen_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ScoreNumber->SetText(FText::AsNumber(0));
}
