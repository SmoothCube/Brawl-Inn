// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/GameModes/GameMode_B.h"
#include "System/Structs/CharacterVariants.h"
#include "MenuGameMode_B.generated.h"

DECLARE_DELEGATE(FPlayersActiveUpdated);

class ULevelSequence;
class UCharacterSelectionOverlay_B;
class UMainMenu_B;
class APlayerCharacter_B;
class AMenuPlayerController_B;
class ASelectionPawn_B;

UCLASS()
class BRAWLINN_API AMenuGameMode_B : public AGameMode_B
{
	GENERATED_BODY()

protected:

	AMenuGameMode_B();
	void PrepareCharacterSelection();

	void PostLevelLoad() override;

	// ********** MainMenu **********

	UFUNCTION(BlueprintCallable)
		void ShowMainMenu();

	UFUNCTION(BlueprintCallable)
		void HideMainMenu();

	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
		TSubclassOf<UMainMenu_B> BP_MainMenu;

	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
		TSubclassOf<ACameraActor> IntroCamera_BP;

	UPROPERTY()
		UMainMenu_B* MainMenuWidget = nullptr;

	// ********** Level Sequences **********

	UPROPERTY(EditDefaultsOnly, Category = "Level Sequence")
		ULevelSequence* IntroLevelSequence;

	UPROPERTY(EditDefaultsOnly, Category = "Level Sequence")
		ULevelSequence* ToGameLevelSequence;

	UPROPERTY(EditDefaultsOnly, Category = "Level Sequence")
		TSubclassOf<ACameraActor> SequenceCamera_BP;

	UFUNCTION()
		void OnIntroLevelSequencePaused();


	// ********** Ready up **********
public:

	void UpdateCharacterSelectionOverlay();

	int GetPlayersActive() const;

	void SetPlayersActive(int Value);

	int GetPlayersReady() const;

	void SetPlayersReady(int Value);

	UFUNCTION()
		void OnMenuPlayButtonClicked();

	UFUNCTION()
		void PrepareGameStart();

	UFUNCTION()
		void OnToGameLevelSequencePaused();

	FPlayersActiveUpdated PlayersActiveUpdated;

protected:
	unsigned int PlayersActive = 0;

	unsigned int PlayersReady = 0;

	// ********** CharacterSelection **********
public:

	void Select(AMenuPlayerController_B* PlayerControllerThatSelected, int Index);

	void UnSelect(AMenuPlayerController_B* PlayerControllerThatSelected);

	void UpdateCharacterVisuals(AMenuPlayerController_B* PlayerController, ASelectionPawn_B* SelectionPawn, int ID);

	void HoverLeft(AMenuPlayerController_B* PlayerController);

	void HoverRight(AMenuPlayerController_B* PlayerController);

	UCharacterSelectionOverlay_B* GetCharacterSelectionOverlay() const;

protected:
	void UpdateOtherSelections();

	UPROPERTY(EditDefaultsOnly, Category = "Character Selection")
		TSubclassOf<UCharacterSelectionOverlay_B> BP_CharacterSelectionOverlay;

	UPROPERTY(BlueprintReadWrite)
		UCharacterSelectionOverlay_B* CharacterSelectionOverlay = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Selection")
		TSubclassOf<ASelectionPawn_B> BP_SelectionPawn;

	UPROPERTY(EditDefaultsOnly, Category = "Selection", meta = (Tooltip = "The location is relative to the playercharacter its supposed to hover over."))
		FVector SelectionIndicatorOffsetLocation = FVector(0, 0, 250);

	UPROPERTY(EditDefaultsOnly, Category = "Selection")
		TArray<FCharacterVariants> CharacterVariants;

	UPROPERTY(EditDefaultsOnly, Category = "Selection")
		TArray<UPaperSprite*> SelectionIndicators;

	UPROPERTY()
		TArray<AMenuPlayerController_B*> MenuPlayerControllers;

	UPROPERTY()
		TArray<APlayerCharacter_B*> Characters;

	TArray<FTransform> CharacterStartTransforms;

};
