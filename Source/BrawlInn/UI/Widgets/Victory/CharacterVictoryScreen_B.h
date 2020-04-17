// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterVictoryScreen_B.generated.h"

class UTextBlock;

UCLASS()
class BRAWLINN_API UCharacterVictoryScreen_B : public UUserWidget
{
	GENERATED_BODY()

	void NativeOnInitialized() override;

protected:
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* ScoreNumber;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* PunchNumber;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* OutOfMapNumber;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* CrownTimeNumber;

};
