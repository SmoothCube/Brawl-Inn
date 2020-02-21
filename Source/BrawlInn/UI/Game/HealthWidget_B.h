// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "System/Structs/ScoreValues.h"
#include "HealthWidget_B.generated.h"

class UProgressBar;
class UVerticalBox;
class UImage;
class UTextBlock;
class APlayerCharacter_B;

UCLASS()
class BRAWLINN_API UHealthWidget_B : public UUserWidget
{
	GENERATED_BODY()
protected:

	virtual void NativeConstruct() override;
	virtual bool Initialize() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UVerticalBox* RespawnCharges;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UProgressBar* HealthProgressBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* ScoreValueText;

	UPROPERTY()
	TArray<UImage*> Barrels;


public:

	UFUNCTION(BlueprintImplementableEvent)
	void PostInitialize(APlayerCharacter_B* Character);

	UFUNCTION()
	void UpdateHealthAmount(int Amount);
	
	void UpdateScoreValues(FScoreValues ScoreValues);

	UFUNCTION()
		void UpdateRespawnsAmount();

};
