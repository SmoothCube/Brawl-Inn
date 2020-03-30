// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsWidget_B.h"
#include "WidgetSwitcher.h"

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

	BackButton->SetAllNavigationRules(EUINavigationRule::Stop,NAME_None);
	BackButton->SetNavigationRuleExplicit(EUINavigation::Up, AudioSettingsWidget->SfxSlider);

	AudioButton->SetNavigationRuleExplicit(EUINavigation::Down, AudioSettingsWidget->MasterSlider);
	AudioButton->ShouldUpdateStyle(false);

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

	//FOnInputAction FaceButtonRightDelegate;
	//FaceButtonRightDelegate.BindDynamic(this, &USettingsWidget_B::Back);
	//ListenForInputAction("FaceButtonRight", IE_Pressed, true, FaceButtonRightDelegate);
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

	if (AudioSettingsWidget->InvisibleButton->HasAnyUserFocus())
		FocusBackButton();
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

	BackButton->SetNavigationRuleBase(EUINavigation::Right, EUINavigationRule::Stop);
	BackButton->SetNavigationRuleExplicit(EUINavigation::Up, AudioSettingsWidget->SfxSlider);

	FocusBackButton();

}

void USettingsWidget_B::OnVideoButtonClicked()
{
	SettingsSwitcher->SetActiveWidgetIndex(1);
	AudioButton->ShouldUpdateStyle(true);
	VideoButton->ShouldUpdateStyle(false);

	BackButton->SetNavigationRuleExplicit(EUINavigation::Right, VideoSettingsWidget->ApplyButton);
	BackButton->SetNavigationRuleExplicit(EUINavigation::Up, VideoSettingsWidget->ScreenResolutionBox);

	FocusBackButton();
}

void USettingsWidget_B::FocusBackButton()
{
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(BackButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
}