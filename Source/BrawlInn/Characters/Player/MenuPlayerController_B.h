// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/PlayerController_B.h"
#include "MenuPlayerController_B.generated.h"

class ASelectionPawn_B;

UCLASS()
class BRAWLINN_API AMenuPlayerController_B : public APlayerController_B
{
	GENERATED_BODY()

	// ********** AActor **********
protected:
	virtual void SetupInputComponent() override;

	// ********** Character Selection **********
public:
	ASelectionPawn_B* GetSelectionPawn();

	int GetSelectedCharacterIndex() const;

	void SetSelectedCharacterIndex(int Value);

protected:
	void TrySelect();
	
	void Unselect();
	
	void SelectLeft();
	
	void SelectRight();

	int SelectedCharacterIndex = 0;

	UPROPERTY()
		ASelectionPawn_B* SelectionPawn;
};
