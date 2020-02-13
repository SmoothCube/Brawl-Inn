// Fill out your copyright notice in the Description page of Project Settings.

#include "VictoryScreenWidget_B.h"
#include "Components/TextBlock.h"
#include "UI/Buttons/Button_B.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Player/PlayerController_B.h"

bool UVictoryScreenWidget_B::Initialize()
{
	bool b = Super::Initialize();

	return b;
}

void UVictoryScreenWidget_B::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerWon->SetText(FText::FromString("OwningCharacter " + FString::FormatAsNumber(UGameplayStatics::GetPlayerControllerID(GetOwningPlayer()) + 1) + " won!"));

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ContinueButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
	ContinueButton->SetKeyboardFocus();

}

void UVictoryScreenWidget_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	ContinueButton->ButtonTick();
	
}
	
