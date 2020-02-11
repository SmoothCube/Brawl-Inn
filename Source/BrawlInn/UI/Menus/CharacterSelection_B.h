// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelection_B.generated.h"

class UTextBlock;

UCLASS()
class BRAWLINN_API UCharacterSelection_B : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateNumberOfPlayersText(int Value);
	void UpdateNumberOfPlayersReady(int Value);
protected:
	// ** Overridden Functions **
	virtual bool Initialize() override;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* NumberOfPlayersText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* NumberOfPlayersReady;

};
