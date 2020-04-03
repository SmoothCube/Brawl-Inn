// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "VictoryScreenWidget_B.generated.h"

class UTextBlock;
class UButton_B;

UCLASS()
class BRAWLINN_API UVictoryScreenWidget_B : public UUserWidget
{
	GENERATED_BODY()
protected:

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
		void ContinueButtonClicked();

	UFUNCTION(BlueprintCallable)
		void EnableContinueButton();

public:
	UPROPERTY(meta = (BindWidget))
		UButton_B* ContinueButton;
};
