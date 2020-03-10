// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverlay_B.generated.h"

class UGameInstance_B;
class UColoredTextBlock_B;
class UTextBlock;

UCLASS()
class BRAWLINN_API UGameOverlay_B : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
		UColoredTextBlock_B* P1;

	UPROPERTY(meta = (BindWidget))
		UColoredTextBlock_B* P2;

	UPROPERTY(meta = (BindWidget))
		UColoredTextBlock_B* P3;

	UPROPERTY(meta = (BindWidget))
		UColoredTextBlock_B* P4;

	UPROPERTY()
		UGameInstance_B* GameInstance = nullptr;


	// ********** Timer **********
public:
	void UpdateTimerText(int TimeRemaining);

	UPROPERTY(meta = (BindWidget))
		UTextBlock* TimeText;
};
