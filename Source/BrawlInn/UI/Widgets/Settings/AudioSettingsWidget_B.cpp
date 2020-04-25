// Fill out your copyright notice in the Description page of Project Settings.

#include "AudioSettingsWidget_B.h"
#include "TextBlock.h"
#include "Engine/Font.h"

#include "SettingsWidget_B.h"
#include "System/GameInstance_B.h"
#include "UI/UIElements/Button_B.h"
#include "UI/UIElements/Slider_B.h"

void UAudioSettingsWidget_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	check(IsValid(GameInstance));

	SfxSlider->OnValueChanged.AddDynamic(this, &UAudioSettingsWidget_B::OnSfxValueChanged);
	MasterSlider->OnValueChanged.AddDynamic(this, &UAudioSettingsWidget_B::OnMasterValueChanged);
	MusicSlider->OnValueChanged.AddDynamic(this, &UAudioSettingsWidget_B::OnMusicValueChanged);

	SfxSlider->SetValue(GameInstance->GetSfxVolume());
	OnSfxValueChanged(GameInstance->GetSfxVolume());
	MasterSlider->SetValue(GameInstance->GetMasterVolume());
	OnMasterValueChanged(GameInstance->GetMasterVolume());
	MusicSlider->SetValue(GameInstance->GetMusicVolume());
	OnMusicValueChanged(GameInstance->GetMusicVolume());

	FSlateFontInfo UnSelectedFontInfo;
	UnSelectedFontInfo.FontObject = TextFont;
	UnSelectedFontInfo.TypefaceFontName = "Default";
	UnSelectedFontInfo.Size = 32;

	const FSlateColor UnSelectedColor(FLinearColor(0.930111f, 0.623960f, 0.147027f, 1));

	FSlateFontInfo SelectedFontInfo;
	SelectedFontInfo.FontObject = TextFont;
	SelectedFontInfo.TypefaceFontName = "Italic";
	SelectedFontInfo.Size = 32;

	const FSlateColor SelectedColor(FLinearColor(1.000000f, 0.896269f, 0.376262f, 1));

	MasterSlider->SetTextAndSettings(MasterText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor);
	MusicSlider->SetTextAndSettings(MusicText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor);
	SfxSlider->SetTextAndSettings(SfxText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor);
	
	UIElementsWithInterface.Add(SfxSlider);
	UIElementsWithInterface.Add(MasterSlider);
	UIElementsWithInterface.Add(MusicSlider);

}

void UAudioSettingsWidget_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	for (UWidget* Element : UIElementsWithInterface)
	{
		if (IsValid(Element) && Element->GetClass()->ImplementsInterface(UUIElementsInterface_B::StaticClass()))
			IUIElementsInterface_B::Execute_Tick(Element);
	}
}

void UAudioSettingsWidget_B::OnSfxValueChanged(float Value)
{
	check(IsValid(GameInstance));
	
	GameInstance->SetSfxVolume(Value);
}

void UAudioSettingsWidget_B::OnMasterValueChanged(float Value)
{
	check(IsValid(GameInstance));
	
	GameInstance->SetMasterVolume(Value);
}

void UAudioSettingsWidget_B::OnMusicValueChanged(float Value)
{
	check(IsValid(GameInstance));
	
	GameInstance->SetMusicVolume(Value);
}
