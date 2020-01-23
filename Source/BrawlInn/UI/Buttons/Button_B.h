// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Styling/SlateBrush.h"
#include "Button_B.generated.h"


UCLASS()
class BRAWLINN_API UButton_B : public UButton
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable)
	void ButtonTick();

protected:

	UPROPERTY(EditAnywhere)
		FButtonStyle MouseHover;

	UPROPERTY(EditAnywhere)
		FButtonStyle Selected;

	UPROPERTY(EditAnywhere)
		FButtonStyle Unselected;
};
