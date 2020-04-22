// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu_B.generated.h"

class UControllerLayout_B;
class USettingsWidget_B;
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
		void ControlsButtonClicked();

	UFUNCTION()
		void ControllerLayoutBackButtonClicked();

	UFUNCTION()
		void CreditsButtonClicked();

	UFUNCTION()
		void QuitButtonClicked();

	UFUNCTION()
		void BackFromSettingsButtonClicked();

	// ********** Widgets **********

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton_B* PlayButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* SettingsButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* CreditsButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* ControlsButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* QuitButton;

	UPROPERTY(meta = (BindWidget))
		UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
		USettingsWidget_B* SettingsWidget;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UControllerLayout_B* ControllerLayout;

	// ********** Misc. **********

	UPROPERTY()
		AMenuGameMode_B* GameMode = nullptr;

	UPROPERTY()
		UGameInstance_B* GameInstance = nullptr;

	UPROPERTY()
		TArray<UWidget*> UIElementsWithInterface;
};
