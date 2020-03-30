// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterSelectionOverlay_B.h"

#include "BrawlInn.h"
#include "Components/TextBlock.h"
#include "ToMainMenuConfirmation_B.h"
#include "UI/UIElements/Button_B.h"

void UCharacterSelectionOverlay_B::UpdateNumberOfPlayersReady(int Value)
{
	if (NumberOfPlayersReady)
		NumberOfPlayersReady->SetText(FText::FromString(FString::FormatAsNumber(Value)));
}

void UCharacterSelectionOverlay_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FOnInputAction InputAction;
	InputAction.BindDynamic(this, &UCharacterSelectionOverlay_B::OnSpecialRightPressed);
	ListenForInputAction("SpecialRight", IE_Pressed, true, InputAction);

}

void UCharacterSelectionOverlay_B::OnSpecialRightPressed()
{
	ToMainMenuConfirmation->SetVisibility(ESlateVisibility::Visible);

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputMode.SetWidgetToFocus(ToMainMenuConfirmation->NoButton->TakeWidget());
	GetOwningPlayer()->SetInputMode(InputMode);

}
