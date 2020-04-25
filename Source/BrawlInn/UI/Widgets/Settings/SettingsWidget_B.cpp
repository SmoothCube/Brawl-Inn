// Fill out your copyright notice in the Description page of Project Settings.

#include "SettingsWidget_B.h"
#include "WidgetSwitcher.h"
#include "Engine/Font.h"

#include "AudioSettingsWidget_B.h"
#include "BrawlInn.h"
#include "UI/UIElements/Button_B.h"
#include "UI/UIElements/ComboBoxString_B.h"
#include "UI/UIElements/Slider_B.h"
#include "VideoSettingsWidget_B.h"

void USettingsWidget_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	AudioButton->OnClicked.AddDynamic(this, &USettingsWidget_B::OnAudioButtonClicked);
	VideoButton->OnClicked.AddDynamic(this, &USettingsWidget_B::OnVideoButtonClicked);

	AudioButton->ShouldUpdateStyle(false);
	VideoButton->ShouldUpdateStyle(true);


	FSlateFontInfo UnSelectedFontInfo;
	UnSelectedFontInfo.FontObject = AbadiMTFont;
	UnSelectedFontInfo.TypefaceFontName = "Light";
	UnSelectedFontInfo.Size = 36;

	const FSlateColor UnSelectedColor(FLinearColor(0.930111f, 0.623960f, 0.147027f, 1));

	FSlateFontInfo SelectedFontInfo;
	SelectedFontInfo.FontObject = AbadiMTFont;
	SelectedFontInfo.TypefaceFontName = "Italic";
	SelectedFontInfo.Size = 36;

	const FSlateColor SelectedColor(FLinearColor(1.000000f, 0.896269f, 0.376262f, 1));

	BackButton->SetTextAndSettings(BackText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor, false);

	const FSlateColor UnSelectedTabColor(FLinearColor(0.147027f, 0.223228f, 0.262251f, 1));
	const FSlateColor SelectedTabColor(FLinearColor(0.564712f, 0.708376f, 0.799103f, 1));

	UnSelectedFontInfo.Size = 20;
	SelectedFontInfo.Size = 20;

	AudioButton->SetTextAndSettings(AudioText, UnSelectedFontInfo, UnSelectedTabColor, SelectedFontInfo, SelectedTabColor, false);
	VideoButton->SetTextAndSettings(VideoText, UnSelectedFontInfo, UnSelectedTabColor, SelectedFontInfo, SelectedTabColor, false);

}

void USettingsWidget_B::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);

	FOnInputAction RightShoulderDelegate;
	RightShoulderDelegate.BindDynamic(this, &USettingsWidget_B::OnVideoButtonClicked);
	ListenForInputAction("RightShoulder", IE_Pressed, true, RightShoulderDelegate);

	FOnInputAction LeftShoulderDelegate;
	LeftShoulderDelegate.BindDynamic(this, &USettingsWidget_B::OnAudioButtonClicked);
	ListenForInputAction("LeftShoulder", IE_Pressed, true, LeftShoulderDelegate);


}

void USettingsWidget_B::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnRemovedFromFocusPath(InFocusEvent);

	StopListeningForAllInputActions();
}

void USettingsWidget_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	BackButton->Execute_Tick(BackButton);
	AudioButton->Execute_Tick(AudioButton);
	VideoButton->Execute_Tick(VideoButton);

}

void USettingsWidget_B::FocusBackButton()
{
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(BackButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
}

UButton_B* USettingsWidget_B::GetBackButton() const
{
	return BackButton;
}


void USettingsWidget_B::BackFromSettingsButtonClicked()
{
	//WidgetSwitcher->SetActiveWidgetIndex(0);

//	FInputModeGameAndUI InputMode;
	//InputMode.SetWidgetToFocus(SettingsButton->GetCachedWidget());
	//GetOwningPlayer()->SetInputMode(InputMode);
}

void USettingsWidget_B::OnAudioButtonClicked()
{
	SettingsSwitcher->SetActiveWidgetIndex(0);
	AudioButton->ShouldUpdateStyle(false);
	VideoButton->ShouldUpdateStyle(true);

	FocusBackButton();
}

void USettingsWidget_B::OnVideoButtonClicked()
{
	SettingsSwitcher->SetActiveWidgetIndex(1);
	AudioButton->ShouldUpdateStyle(true);
	VideoButton->ShouldUpdateStyle(false);

	FocusBackButton();
}