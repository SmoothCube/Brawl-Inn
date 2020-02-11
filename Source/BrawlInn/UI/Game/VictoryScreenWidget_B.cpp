// Fill out your copyright notice in the Description page of Project Settings.

#include "VictoryScreenWidget_B.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Player/PlayerController_B.h"

bool UVictoryScreenWidget_B::Initialize()
{
	return Super::Initialize();
}

void UVictoryScreenWidget_B::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerWon->SetText(FText::FromString("Player " + FString::FormatAsNumber(UGameplayStatics::GetPlayerControllerID(GetOwningPlayer()) + 1) + " won!"));

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ContinueButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
}
