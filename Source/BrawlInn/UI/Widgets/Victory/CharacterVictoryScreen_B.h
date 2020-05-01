// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterVictoryScreen_B.generated.h"

class UImage;
class UVerticalBox;
class UTextBlock;

UCLASS()
class BRAWLINN_API UCharacterVictoryScreen_B : public UUserWidget
{
	GENERATED_BODY()

		void NativeOnInitialized() override;

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UVerticalBox* StatsBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* ScoreNumber;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* CharacterRender;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* BannerBot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* BannerTop;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* BackgroundColor;
public:
	UPROPERTY()
		UMaterialInstanceDynamic* BackgroundMaterial = nullptr;

};
