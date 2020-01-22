// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenu_B.generated.h"

class UButton;

UCLASS()
class BRAWLINN_API UPauseMenu_B : public UUserWidget
{
	GENERATED_BODY()

protected:
	// ** Overridden Functions **
	virtual bool Initialize() override;

	// ** Delegates **
	UFUNCTION()
		void ContinueButtonClicked();

	UFUNCTION()
		void ExitButtonClicked();

	UPROPERTY(meta = (BindWidget))
		UButton* ContinueButton;

	UPROPERTY(meta = (BindWidget))
		UButton* ExitButton;
};
