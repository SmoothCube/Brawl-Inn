// Fill out your copyright notice in the Description page of Project Settings.

#include "AudioSettingsWidget_B.h"
#include "TextBlock.h"

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
	SfxValueText->SetText(FText::FromString(FString::FormatAsNumber(Value * 100) + FString("%")));
}

void UAudioSettingsWidget_B::OnMasterValueChanged(float Value)
{
	check(IsValid(GameInstance));
	
	GameInstance->SetMasterVolume(Value);
	MasterValueText->SetText(FText::FromString(FString::FormatAsNumber(Value * 100) + FString("%")));
}

void UAudioSettingsWidget_B::OnMusicValueChanged(float Value)
{
	check(IsValid(GameInstance));
	
	GameInstance->SetMusicVolume(Value);
	MusicValueText->SetText(FText::FromString(FString::FormatAsNumber(Value * 100) + FString("%")));
}
