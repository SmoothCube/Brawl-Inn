// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VideoSettingsWidget_B.generated.h"

class UComboBoxString;
class UButton_B;

UCLASS()
class BRAWLINN_API UVideoSettingsWidget_B : public UUserWidget
{
	GENERATED_BODY()

protected:
	void NativeOnInitialized() override;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
		void OnScreenResolutionBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
		void OnFullscreenModeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
		void OnApplyButtonClicked();

	UPROPERTY(meta = (BindWidget))
		UComboBoxString* FullscreenModeBox;

	UPROPERTY(meta = (BindWidget))
		UComboBoxString* ScreenResolutionBox;

public:
	UPROPERTY(meta = (BindWidget))
		UButton_B* ApplyButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* BackButton;

protected:
	TMap<FString, FIntPoint> ScreenResolutions;
};
