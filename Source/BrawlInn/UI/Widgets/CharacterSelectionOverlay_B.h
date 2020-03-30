// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectionOverlay_B.generated.h"

class UToMainMenuConfirmation_B;
class UTextBlock;

UCLASS()
class BRAWLINN_API UCharacterSelectionOverlay_B : public UUserWidget
{
	GENERATED_BODY()
protected:
	// ********** UUserWidget **********
	void NativeOnInitialized() override;

	// ********** Widgets **********

	UPROPERTY(meta = (BindWidget))
		UTextBlock* NumberOfPlayersReady;

	UPROPERTY(meta = (BindWidget))
		UToMainMenuConfirmation_B* ToMainMenuConfirmation;

	// ********** Functions **********
public:
	void UpdateNumberOfPlayersReady(int Value);

protected:
	// ********** Button presses **********
	UFUNCTION()
		void OnSpecialRightPressed();
};