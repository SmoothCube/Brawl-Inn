// Fill out your copyright notice in the Description page of Project Settings.

#include "VictoryScreenWidget_B.h"
#include "Components/TextBlock.h"
#include "UI/UIElements/Button_B.h"
#include "Kismet/GameplayStatics.h"
#include "System/GameInstance_B.h"

void UVictoryScreenWidget_B::NativeConstruct()
{
	Super::NativeConstruct();

}

void UVictoryScreenWidget_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	ContinueButton->Execute_Tick(ContinueButton);
}

void UVictoryScreenWidget_B::ContinueButtonClicked()
{
	UGameInstance_B* GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	check(IsValid(GameInstance));
	GameInstance->ReturnToMainMenu();
}

void UVictoryScreenWidget_B::EnableContinueButton()
{
	ContinueButton->OnClicked.AddDynamic(this, &UVictoryScreenWidget_B::ContinueButtonClicked);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ContinueButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
	ContinueButton->SetKeyboardFocus();
}
