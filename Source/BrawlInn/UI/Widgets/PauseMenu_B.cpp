// Fill out your copyright notice in the Description page of Project Settings.

#include "PauseMenu_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Font.h"

#include "BrawlInn.h"
#include "ConfigCacheIni.h"
#include "ControllerLayout_B.h"
#include "Settings/SettingsWidget_B.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/GameMode_B.h"
#include "UI/UIElements/Button_B.h"

void UPauseMenu_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FSlateFontInfo UnSelectedFontInfo;
	UnSelectedFontInfo.FontObject = TextFont;
	UnSelectedFontInfo.TypefaceFontName = "Light";
	UnSelectedFontInfo.Size = 42;

	const FSlateColor UnSelectedColor(FLinearColor(0.930111f, 0.623960f, 0.147027f, 1));

	FSlateFontInfo SelectedFontInfo;
	SelectedFontInfo.FontObject = TextFont;
	SelectedFontInfo.TypefaceFontName = "Italic";
	SelectedFontInfo.Size = 42;

	const FSlateColor SelectedColor(FLinearColor(1.000000f, 0.896269f, 0.376262f, 1));

	ContinueButton->SetTextAndSettings(ContinueText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor, true);
	ControlsButton->SetTextAndSettings(ControlsText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor, true);

	ExitButton->SetTextAndSettings(ExitText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor, true);

	UIElementsWithInterface.Add(ContinueButton);
	UIElementsWithInterface.Add(ControlsButton);
	UIElementsWithInterface.Add(ExitButton);
	
}

void UPauseMenu_B::NativeConstruct()
{
	Super::NativeConstruct();

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ContinueButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
	ContinueButton->SetKeyboardFocus();

	ContinueButton->OnClicked.AddDynamic(this, &UPauseMenu_B::ContinueButtonClicked);
	ControlsButton->OnClicked.AddDynamic(this, &UPauseMenu_B::ControlsButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &UPauseMenu_B::ExitButtonClicked);

	ControllerLayout->SetVisibility(ESlateVisibility::Collapsed);
}

void UPauseMenu_B::RemoveFromParent()
{
	Super::RemoveFromParent();

	if (GetOwningPlayer())
	{
		const FInputModeGameOnly InputMode;
		GetOwningPlayer()->SetInputMode(InputMode);
	}
}

void UPauseMenu_B::MenuTick()
{
	for (UWidget* Element : UIElementsWithInterface)
	{
		if (IsValid(Element) && Element->GetClass()->ImplementsInterface(UUIElementsInterface_B::StaticClass()))
			IUIElementsInterface_B::Execute_Tick(Element);
	}
}

void UPauseMenu_B::ContinueButtonClicked()
{
	AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	check(IsValid(GameMode));

	GameMode->ResumeGame();
}

void UPauseMenu_B::ControlsButtonClicked()
{
	ControllerLayout->SetVisibility(ESlateVisibility::Visible);

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(ControllerLayout->GetBackButton()->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);

	ControllerLayout->GetBackButton()->OnPressed.AddDynamic(this, &UPauseMenu_B::ControllerLayoutBackButtonClicked);
}

void UPauseMenu_B::ControllerLayoutBackButtonClicked()
{
	ControllerLayout->SetVisibility(ESlateVisibility::Collapsed);

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(ControlsButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);

	ControllerLayout->GetBackButton()->OnPressed.RemoveDynamic(this, &UPauseMenu_B::ControllerLayoutBackButtonClicked);
}

void UPauseMenu_B::ExitButtonClicked()
{
	UGameInstance_B* GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	check(IsValid(GameInstance));
	GameInstance->ReturnToMainMenu();
}
