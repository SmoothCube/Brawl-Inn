// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "VictoryScreenWidget_B.generated.h"

class UImage;
class UTextBlock;
class APlayerController_B;

UCLASS()
class BRAWLINN_API UVictoryScreenWidget_B : public UUserWidget
{
	GENERATED_BODY()
protected:

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void OnPlayerWin(APlayerController_B* PlayerThatWon);
protected:
	UPROPERTY(meta = (BindWidget))
		UImage* VictoryImage;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* PlayerWon;
};
