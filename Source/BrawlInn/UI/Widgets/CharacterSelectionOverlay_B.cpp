// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterSelectionOverlay_B.h"
#include "Components/TextBlock.h"

void UCharacterSelectionOverlay_B::UpdateNumberOfPlayersReady(int Value)
{
	if (NumberOfPlayersReady)
		NumberOfPlayersReady->SetText(FText::FromString(FString::FormatAsNumber(Value)));
}