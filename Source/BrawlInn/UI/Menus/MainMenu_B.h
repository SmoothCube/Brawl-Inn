// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu_B.generated.h"

class UButton;

UCLASS()
class BRAWLINN_API UMainMenu_B : public UUserWidget
{
	GENERATED_BODY()

protected:
	// ** Overridden Functions **
	virtual bool Initialize() override;

	// ** Delegates **
	UFUNCTION()
	void PlayButtonClicked();

	UFUNCTION()
		void SettingsButtonClicked();

	UFUNCTION()
		void CreditsButtonClicked();

	UFUNCTION()
		void QuitButtonClicked();
	
	// ** Buttons **

	UPROPERTY(meta = (BindWidget))
		UButton* PlayButton;

	UPROPERTY(meta = (BindWidget))
		UButton* SettingsButton;

	UPROPERTY(meta = (BindWidget))
		UButton* CreditsButton;

	UPROPERTY(meta = (BindWidget))
		UButton* QuitButton;


};
