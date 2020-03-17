// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Slider.h"
#include "UI/UIElements/UIElementsInterface_B.h"
#include "Slider_B.generated.h"

UCLASS()
class BRAWLINN_API USlider_B : public USlider, public IUIElementsInterface_B
{
	GENERATED_BODY()

	void OnCreationFromPalette() override;

	virtual void Tick_Implementation() override;

	UPROPERTY(EditAnywhere, Category = "Brawl Inn Settings")
	FLinearColor SelectedHandle;

		UPROPERTY(EditAnywhere, Category = "Brawl Inn Settings")
	FLinearColor UnselectedHandle;
};
