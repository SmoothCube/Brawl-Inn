// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/GamePlayerController_B.h"
#include "MenuPlayerController_B.generated.h"

class ASelectionPawn_B;

UCLASS()
class BRAWLINN_API AMenuPlayerController_B : public AGamePlayerController_B
{
	GENERATED_BODY()

	// ********** AActor **********
protected:

	void OnPossess(APawn* InPawn) override;

	void OnUnPossess() override;

	// ********** Character Selection **********
public:
	ASelectionPawn_B* GetSelectionPawn();

	int GetCharacterVariantIndex() const;

	void SetCharacterVariantIndex(int Value);

protected:

	void LeftStickRightPressed() override;

	void LeftStickLeftPressed() override;

	void LeftShoulderPressed() override;

	void DPadLeftPressed() override;

	void DPadRightPressed() override;
	
	void RightShoulderPressed() override;

	void FaceButtonBottomPressed() override;

	void FaceButtonRightPressed() override;
	
	void TrySelect();
	
	void Unselect();
	
	void SelectLeft();
	
	void SelectRight();

	int CharacterVariantIndex = 0;

	UPROPERTY()
		ASelectionPawn_B* SelectionPawn;
};
