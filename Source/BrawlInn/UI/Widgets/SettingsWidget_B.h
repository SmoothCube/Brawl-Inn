// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsWidget_B.generated.h"

class UAudioSettingsWidget_B;
class UWidgetSwitcher;
class UVideoSettingsWidget_B;
class UButton_B;

UCLASS()
class BRAWLINN_API USettingsWidget_B : public UUserWidget
{
	GENERATED_BODY()
protected:
	// ********** UUserWidget **********
	void NativeOnInitialized() override;

	void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;

	void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	// ********** Button clicks **********

	UFUNCTION()
		void OnAudioButtonClicked();

	UFUNCTION()
		void OnVideoButtonClicked();

	UFUNCTION()
		void BackFromSettingsButtonClicked();

	void FocusBackButton();
protected:
	// ********** Widgets **********
	UPROPERTY(meta = (BindWidget))
		UWidgetSwitcher* SettingsSwitcher;

	UPROPERTY(meta = (BindWidget))
		UAudioSettingsWidget_B* AudioSettingsWidget;

	UPROPERTY(meta = (BindWidget))
		UVideoSettingsWidget_B* VideoSettingsWidget;

	UPROPERTY(meta = (BindWidget))
		UButton_B* AudioButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* VideoButton;

public:
	UPROPERTY(meta = (BindWidget))
		UButton_B* BackButton;
};
