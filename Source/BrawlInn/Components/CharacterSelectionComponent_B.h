// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterSelectionComponent_B.generated.h"

DECLARE_DELEGATE(FCharacterSelected);
DECLARE_DELEGATE(FCharacterUnselected);

class APlayerCharacter_B;
class APlayerController_B;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRAWLINN_API UCharacterSelectionComponent_B : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterSelectionComponent_B();

		FCharacterSelected CharacterSelected_D;

		FCharacterUnselected CharacterUnselected_D;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	

	void SelectCharacter(APlayerController_B* PlayerController);

	void Unselect(APlayerController_B* PlayerController);

	void NextCharacter(APlayerController_B* PlayerController);

private:

	UPROPERTY()
	APlayerCharacter_B* CharacterSelected = nullptr;
	
	int CurrentIndex = 0;
	bool bFirstTime = true;

	TArray<APlayerCharacter_B*> Characters;
	TArray<APlayerCharacter_B*> SelectedCharacters;
	TMap<APlayerCharacter_B*, FTransform> StartLocations;
};
