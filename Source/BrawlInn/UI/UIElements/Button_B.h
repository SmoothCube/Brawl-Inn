// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "System/Interfaces/UIElementsInterface_B.h"
#include "Button_B.generated.h"

UCLASS()
class BRAWLINN_API UButton_B : public UButton, public IUIElementsInterface_B
{
	GENERATED_BODY()


public:

	void Tick_Implementation() override;

	UPROPERTY(EditAnywhere)
		FButtonStyle Selected;

	UPROPERTY(EditAnywhere)
		FButtonStyle Pressed;
	
	UPROPERTY(EditAnywhere)
		FButtonStyle Unselected;

	void ShouldUpdateStyle(bool bShouldUpdateStyleIn);
protected:
	bool bShouldUpdateStyle = true;
};
