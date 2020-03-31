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

	// ********** UUserWidget **********
protected:
	void NativeOnInitialized() override;
	
	void NativeConstruct() override;
public:
	void RemoveFromParent() override;
protected:
	// ********** Widgets **********

	UPROPERTY(meta = (BindWidget))
		UButton_B* ContinueButton;
	
	UPROPERTY(meta = (BindWidget))
		UButton_B* ExitButton;

	// ********** Functions **********
public:

	void MenuTick();

protected:
	// ********** Button clicks **********

	UFUNCTION()
		void ContinueButtonClicked();

	UFUNCTION()
		void ExitButtonClicked();

};
