// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsWidget_B.generated.h"

class UTextBlock;
class UAudioSettingsWidget_B;
class UWidgetSwitcher;
class UVideoSettingsWidget_B;
class UButton_B;
class UFont;

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

	UPROPERTY(meta = (BindWidget))
		UButton_B* BackButton;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* BackText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* VideoText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* AudioText;
	
	// ********** Functions **********
	
	void FocusBackButton();
public:
	UButton_B* GetBackButton() const;

	// ********** Button presses **********
protected:
	UFUNCTION()
		void OnAudioButtonClicked();

	UFUNCTION()
		void OnVideoButtonClicked();

	// ********** Text **********

	UPROPERTY(EditDefaultsOnly, Category = "Fonts")
	UFont* AbadiMTFont = nullptr;
};
