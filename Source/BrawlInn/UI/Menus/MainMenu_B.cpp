// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu_B.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "BrawlInn.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/MenuGameMode_B.h"
#include "UI/Buttons/Button_B.h"
#include "UI/Widgets/SettingsWidget_B.h"

void UMainMenu_B::NativeConstruct()
{
	Super::NativeConstruct();

	GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	check(IsValid(GameMode));
	check(IsValid(GameInstance));

	
	PlayButton->OnClicked.AddDynamic(GameMode, &AMenuGameMode_B::PlayButtonClicked);
	SettingsButton->OnClicked.AddDynamic(this, &UMainMenu_B::SettingsButtonClicked);
	SettingsWidget->BackButton->OnClicked.AddDynamic(this, &UMainMenu_B::BackFromSettingsButtonClicked);
	CreditsButton->OnClicked.AddDynamic(this, &UMainMenu_B::CreditsButtonClicked);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu_B::QuitButtonClicked);


	UIElementsWithInterface.Add(PlayButton);
	UIElementsWithInterface.Add(SettingsButton);
	UIElementsWithInterface.Add(CreditsButton);
	UIElementsWithInterface.Add(QuitButton);


	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(PlayButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
}

void UMainMenu_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	for (UWidget* Element : UIElementsWithInterface)
	{
		if (IsValid(Element) && Element->GetClass()->ImplementsInterface(UUIElementsInterface_B::StaticClass()))
			IUIElementsInterface_B::Execute_Tick(Element);
	}
}

void UMainMenu_B::SettingsButtonClicked()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
	
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(SettingsWidget->BackButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
}

void UMainMenu_B::CreditsButtonClicked()
{
	BScreen("Credits");
}

void UMainMenu_B::QuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

void UMainMenu_B::BackFromSettingsButtonClicked()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(SettingsButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
}



void UMainMenu_B::VideoSettingsBackButtonClicked()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(CreditsButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
}