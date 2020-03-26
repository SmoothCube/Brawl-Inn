// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu_B.h"
#include "Components/WidgetSwitcher.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "BrawlInn.h"
#include "UI/UIElements/Slider_B.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/MenuGameMode_B.h"
#include "TextBlock.h"
#include "UI/Buttons/Button_B.h"
#include "UI/Widgets/VideoSettingsWidget_B.h"

void UMainMenu_B::NativeConstruct()
{
	Super::NativeConstruct();

	GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	check(IsValid(GameMode));
	
	PlayButton->OnClicked.AddDynamic(GameMode, &AMenuGameMode_B::PlayButtonClicked);
	SettingsButton->OnClicked.AddDynamic(this, &UMainMenu_B::SettingsButtonClicked);
	BackFromSettingsButton->OnClicked.AddDynamic(this, &UMainMenu_B::BackFromSettingsButtonClicked);
	CreditsButton->OnClicked.AddDynamic(this, &UMainMenu_B::CreditsButtonClicked);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu_B::QuitButtonClicked);

	VideoSettingsWidget->BackButton->OnClicked.AddDynamic(this, &UMainMenu_B::VideoSettingsBackButtonClicked);

	UIElementsWithInterface.Add(PlayButton);
	UIElementsWithInterface.Add(SettingsButton);
	UIElementsWithInterface.Add(CreditsButton);
	UIElementsWithInterface.Add(QuitButton);
	UIElementsWithInterface.Add(BackFromSettingsButton);

	SfxSlider->OnValueChanged.AddDynamic(this, &UMainMenu_B::OnSfxValueChanged);
	MasterSlider->OnValueChanged.AddDynamic(this, &UMainMenu_B::OnMasterValueChanged);
	MusicSlider->OnValueChanged.AddDynamic(this, &UMainMenu_B::OnMusicValueChanged);

	SfxSlider->SetValue(GameInstance->GetSfxVolume());
	OnSfxValueChanged(GameInstance->GetSfxVolume());
	MasterSlider->SetValue(GameInstance->GetMasterVolume());
	OnMasterValueChanged(GameInstance->GetMasterVolume());
	MusicSlider->SetValue(GameInstance->GetMusicVolume());
	OnMusicValueChanged(GameInstance->GetMusicVolume());

	UIElementsWithInterface.Add(SfxSlider);
	UIElementsWithInterface.Add(MasterSlider);
	UIElementsWithInterface.Add(MusicSlider);

	FInputModeUIOnly InputMode;
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

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(BackFromSettingsButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
}

void UMainMenu_B::CreditsButtonClicked()
{
	WidgetSwitcher->SetActiveWidgetIndex(2);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(VideoSettingsWidget->ApplyButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
}

void UMainMenu_B::QuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

void UMainMenu_B::BackFromSettingsButtonClicked()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(SettingsButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
}

void UMainMenu_B::OnSfxValueChanged(const float Value)
{
	if (IsValid(GameInstance))
		GameInstance->SetSfxVolume(Value);

	SfxValueText->SetText(FText::FromString(FString::FormatAsNumber(Value * 100) + FString("%")));
}

void UMainMenu_B::OnMasterValueChanged(const float Value)
{
	if (IsValid(GameInstance))
		GameInstance->SetMasterVolume(Value);

	MasterValueText->SetText(FText::FromString(FString::FormatAsNumber(Value * 100) + FString("%")));

}

void UMainMenu_B::OnMusicValueChanged(const float Value)
{
	if (IsValid(GameInstance))
		GameInstance->SetMusicVolume(Value);

	MusicValueText->SetText(FText::FromString(FString::FormatAsNumber(Value * 100) + FString("%")));

}

void UMainMenu_B::VideoSettingsBackButtonClicked()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(CreditsButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
}