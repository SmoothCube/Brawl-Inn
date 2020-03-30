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
	// ********** UUserWidget **********
	void NativeOnInitialized() override;
	
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ********** Widgets **********
	UPROPERTY(meta = (BindWidget))
		UButton_B* YesButton;
	
	UPROPERTY(meta = (BindWidget))
		UButton_B* NoButton;

public:
	// ********** Functions **********
	UButton_B* GetNoButton() const;

protected:
	// ********** Buttons presses **********
	UFUNCTION()
		void OnYesButtonClicked();

	UFUNCTION()
		void OnNoButtonClicked();
};
