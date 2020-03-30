// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToMainMenuConfirmation_B.generated.h"

class UButton_B;

UCLASS()
class BRAWLINN_API UToMainMenuConfirmation_B : public UUserWidget
{
	GENERATED_BODY()
protected:

	void NativeOnInitialized() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;

public:
	UPROPERTY(meta = (BindWidget))
		UButton_B* YesButton;
	
	UPROPERTY(meta = (BindWidget))
		UButton_B* NoButton;

	UFUNCTION()
		void OnYesButtonClicked();

	UFUNCTION()
		void OnNoButtonClicked();
};
