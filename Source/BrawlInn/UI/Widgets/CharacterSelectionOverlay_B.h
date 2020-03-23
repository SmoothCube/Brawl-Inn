// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectionOverlay_B.generated.h"

class UTextBlock;

UCLASS()
class BRAWLINN_API UCharacterSelectionOverlay_B : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateNumberOfPlayersReady(int Value);

protected:

	UPROPERTY(meta = (BindWidget))
		UTextBlock* NumberOfPlayersReady;
};
