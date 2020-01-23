// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenu_B.generated.h"

class UButton_B;

UCLASS()
class BRAWLINN_API UPauseMenu_B : public UUserWidget
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
		void ContinueButtonClicked();

	UFUNCTION()
		void ExitButtonClicked();

	UPROPERTY(meta = (BindWidget))
		UButton_B* ContinueButton;

	UPROPERTY(meta = (BindWidget))
		UButton_B* ExitButton;

private:

	TArray<UButton_B*> Buttons;
};
