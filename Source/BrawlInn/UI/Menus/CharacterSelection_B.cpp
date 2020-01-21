// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelection_B.h"
#include "Components/TextBlock.h"

bool UCharacterSelection_B::Initialize()
{
	bool success = Super::Initialize();

	return success;
}

void UCharacterSelection_B::UpdateNumberOfPlayersText(int Value)
{
	NumberOfPlayersText->SetText(FText::FromString(FString("Number of players active: ") + FString::FormatAsNumber(Value)));
}