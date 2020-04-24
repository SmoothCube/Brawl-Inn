// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "System/Interfaces/UIElementsInterface_B.h"
#include "Button_B.generated.h"

class UTextBlock;
UCLASS()
class BRAWLINN_API UButton_B : public UButton, public IUIElementsInterface_B
{
	GENERATED_BODY()

public:

	void Tick_Implementation() override;

	void SetTextAndSettings(UTextBlock* Text, FSlateFontInfo UnSelectedFontInfo, FSlateColor UnSelectedColorIn, FSlateFontInfo SelectedFontInfo, FSlateColor
	                        SelectedColorIn);

	void ShouldUpdateStyle(bool bShouldUpdateStyleIn);
protected:
	bool bShouldUpdateStyle = false;

	UPROPERTY()
		UTextBlock* ButtonText;

	FSlateFontInfo UnSelectedFont;
	FSlateColor UnSelectedColor;
	
	FSlateFontInfo SelectedFont;
	FSlateColor SelectedColor;
};
