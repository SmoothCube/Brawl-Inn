// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu_B.generated.h"

class UButton_B;
class AMenuGameMode_B;

UCLASS()
class BRAWLINN_API UMainMenu_B : public UUserWidget
{
	GENERATED_BODY()
		
public:
	void MenuTick();

protected:
	// ** Overridden Functions **
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

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
		UButton_B* PlayButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* SettingsButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* CreditsButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* QuitButton;

	// ** Variables **

	AMenuGameMode_B* GameMode = nullptr;

private:
	TArray<UButton_B*> Buttons;
};
