// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelection_B.generated.h"

class UTextBlock;
class AMenuGameMode_B;

UCLASS()
class BRAWLINN_API UCharacterSelection_B : public UUserWidget
{
	GENERATED_BODY()

protected:
	// ** Overridden Functions **
	virtual bool Initialize() override;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* NumberOfPlayersText;

	UFUNCTION()
	void UpdateNumberOfPlayersText();
	
	// ** Variables **

	AMenuGameMode_B* GameMode = nullptr;
};
