// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenu_B.generated.h"

class UTextBlock;
class UControllerLayout_B;
class UButton_B;
class UFont;

UCLASS()
class BRAWLINN_API UPauseMenu_B : public UUserWidget
{
	GENERATED_BODY()

		// ********** UUserWidget **********
protected:
	void NativeOnInitialized() override;

	void NativeConstruct() override;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	// ********** Widgets **********

	UPROPERTY(meta = (BindWidget))
		UButton_B* ContinueButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* ControlsButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* ExitButton;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* ContinueText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* ControlsText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* ExitText;

	UPROPERTY(meta = (BindWidget))
		UControllerLayout_B* ControllerLayout;

	// ********** Button clicks **********

	UFUNCTION(BlueprintImplementableEvent)
		void OnPauseMenuHide();

	UFUNCTION(BlueprintCallable)
	void OnPauseMenuHideFinished();
	
	UFUNCTION()
		void ContinueButtonClicked();

	UFUNCTION()
		void ControlsButtonClicked();

	UFUNCTION()
		void ControllerLayoutBackButtonClicked();

	UFUNCTION()
		void ExitButtonClicked();

	// ********** Misc. **********

	UPROPERTY(EditDefaultsOnly)
		UFont* TextFont;

	UPROPERTY()
		TArray<UWidget*> UIElementsWithInterface;

};
