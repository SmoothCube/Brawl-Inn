// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ComboBoxString.h"
#include "System/Interfaces/UIElementsInterface_B.h"
#include "ComboBoxString_B.generated.h"


UCLASS()
class BRAWLINN_API UComboBoxString_B : public UComboBoxString, public IUIElementsInterface_B
{
	GENERATED_BODY()
protected:

	void Tick_Implementation() override;
	
	UPROPERTY(EditAnywhere)
		FButtonStyle Selected;

	UPROPERTY(EditAnywhere)
		FButtonStyle Pressed;

	UPROPERTY(EditAnywhere)
		FButtonStyle Unselected;
};
