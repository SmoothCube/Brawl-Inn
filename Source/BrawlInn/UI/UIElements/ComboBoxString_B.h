// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ComboBoxString.h"
#include "System/Interfaces/UIElementsInterface_B.h"
#include "ComboBoxString_B.generated.h"


class UTextBlock;
UCLASS()
class BRAWLINN_API UComboBoxString_B : public UComboBoxString, public IUIElementsInterface_B
{
	GENERATED_BODY()

public:
	void SetTextAndSettings(UTextBlock* Text, FSlateFontInfo UnSelectedFontInfo, FSlateColor UnSelectedColorIn, FSlateFontInfo SelectedFontInfo, FSlateColor
		SelectedColorIn);
protected:

	void Tick_Implementation() override;

	// ********** Text Settings **********

	UPROPERTY()
		UTextBlock* BoxText = nullptr;

	FSlateFontInfo UnSelectedFont;
	FSlateColor UnSelectedColor;

	FSlateFontInfo SelectedFont;
	FSlateColor SelectedColor;
};
