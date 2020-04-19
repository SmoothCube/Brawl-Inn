// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "System/Structs/ScoreValues.h"
#include "VictoryScreenWidget_B.generated.h"

class UStatEntry_B;
class UGameInstance_B;
class UCharacterVictoryScreen_B;
class UTextBlock;
class UButton_B;

UCLASS()
class BRAWLINN_API UVictoryScreenWidget_B : public UUserWidget
{
	GENERATED_BODY()
protected:

	virtual void NativeOnInitialized() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
		void ContinueButtonClicked();

	UFUNCTION(BlueprintCallable)
		void EnableContinueButton();

	void DisplayScores(EScoreValueTypes Type);

	void CountNumber(int Start, int End, UTextBlock* TextBlock);

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton_B* ContinueButton;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UCharacterVictoryScreen_B* StatTopLeft;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UCharacterVictoryScreen_B* StatTopRight;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UCharacterVictoryScreen_B* StatBotLeft;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UCharacterVictoryScreen_B* StatBotRight;

	// ********** Arrays of Stats **********
	UPROPERTY(BlueprintReadWrite)
	TArray<UCharacterVictoryScreen_B*> StatBoards;

	UPROPERTY()
	UGameInstance_B* GameInstance = nullptr;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UStatEntry_B> BP_StatEntry;
	
	
};
