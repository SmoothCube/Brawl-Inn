// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Slider.h"
#include "System/Interfaces/UIElementsInterface_B.h"
#include "Slider_B.generated.h"

class UTextBlock;
UCLASS()
class BRAWLINN_API USlider_B : public USlider, public IUIElementsInterface_B
{
	GENERATED_BODY()

#if WITH_EDITOR
		void OnCreationFromPalette() override;
#endif

	virtual void Tick_Implementation() override;

public:
	void SetTextAndSettings(UTextBlock* Text, FSlateFontInfo UnSelectedFontInfo, FSlateColor UnSelectedColorIn, FSlateFontInfo SelectedFontInfo, FSlateColor
		SelectedColorIn);
protected:

	UPROPERTY(EditAnywhere, Category = "Brawl Inn Settings")
		FSlateBrush UnSelectedNormalBarImage;

	UPROPERTY(EditAnywhere, Category = "Brawl Inn Settings")
	FSlateBrush UnSelectedNormalThumbImage;

	UPROPERTY(EditAnywhere, Category = "Brawl Inn Settings")
		FSlateBrush SelectedNormalBarImage;

	UPROPERTY(EditAnywhere, Category = "Brawl Inn Settings")
		FSlateBrush SelectedNormalThumbImage;

	// ********** Text Settings **********

	UPROPERTY()
		UTextBlock* SliderText = nullptr;

	FSlateFontInfo UnSelectedFont;
	FSlateColor UnSelectedColor;

	FSlateFontInfo SelectedFont;
	FSlateColor SelectedColor;
};
