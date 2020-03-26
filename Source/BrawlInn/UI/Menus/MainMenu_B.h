// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu_B.generated.h"

class UTextBlock;
class IUIElementsInterface_B;
class USlider_B;
class UGameInstance_B;
class UWidgetSwitcher;
class UButton_B;
class AMenuGameMode_B;
class UVideoSettingsWidget_B;

UCLASS()
class BRAWLINN_API UMainMenu_B : public UUserWidget
{
	GENERATED_BODY()

protected:
	// ********** UUserWidget **********

	void NativeConstruct() override;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ********** Button Clicks **********
	UFUNCTION()
		void SettingsButtonClicked();

	UFUNCTION()
		void CreditsButtonClicked();

	UFUNCTION()
		void QuitButtonClicked();

	UFUNCTION()
		void BackFromSettingsButtonClicked();

	UFUNCTION()
		void VideoSettingsBackButtonClicked();

	// ********** Sliders **********

	UFUNCTION()
		void OnSfxValueChanged(float Value);

	UFUNCTION()
		void OnMasterValueChanged(float Value);

	UFUNCTION()
		void OnMusicValueChanged(float Value);


	// ********** Widgets **********

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton_B* PlayButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* SettingsButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* CreditsButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* QuitButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* BackFromSettingsButton;

	UPROPERTY(meta = (BindWidget))
		UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
		UVideoSettingsWidget_B* VideoSettingsWidget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		USlider_B* MasterSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		USlider_B* MusicSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		USlider_B* SfxSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* SfxValueText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* MasterValueText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* MusicValueText;
	// ********** Misc. **********

	UPROPERTY()
		AMenuGameMode_B* GameMode = nullptr;

	UPROPERTY()
		UGameInstance_B* GameInstance = nullptr;

	TArray<UWidget*> UIElementsWithInterface;
};
