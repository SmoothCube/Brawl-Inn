// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "VictoryScreenWidget_B.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class BRAWLINN_API UVictoryScreenWidget_B : public UUserWidget
{
	GENERATED_BODY()
protected:

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;
protected:

	UPROPERTY(meta = (BindWidget))
		UTextBlock* PlayerWon;

	UPROPERTY(meta = (BindWidget))
		UButton* ContinueButton;
};
