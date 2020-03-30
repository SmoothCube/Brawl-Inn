// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VideoSettingsWidget_B.generated.h"

class UComboBoxString_B;
class UButton_B;

UCLASS()
class BRAWLINN_API UVideoSettingsWidget_B : public UUserWidget
{
	GENERATED_BODY()

protected:
	// ********** UUserWidget**********

	void NativeOnInitialized() override;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ********** Widgets **********
	UPROPERTY(meta = (BindWidget))
		UComboBoxString_B* FullscreenModeBox;

	UPROPERTY(meta = (BindWidget))
		UComboBoxString_B* ScreenResolutionBox;

	UPROPERTY(meta = (BindWidget))
		UButton_B* ApplyButton;

public:
	// ********** Functions **********

	UButton_B* GetApplyButton() const;

	UComboBoxString_B* GetScreenResolutionBox() const;

protected:
	// ********** Button presses **********

	UFUNCTION()
		void OnScreenResolutionBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
		void OnFullscreenModeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
		void OnApplyButtonClicked();

	// ********** Misc. **********

	TMap<FString, FIntPoint> ScreenResolutions;

	UPROPERTY()
		TArray<UWidget*> UIElementsWithInterface;
};
