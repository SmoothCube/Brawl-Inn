// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/GameMode_B.h"
#include "MenuGameMode_B.generated.h"

class ACameraActor;
class ULevelSequence;
class ALevelSequenceActor;
class UCharacterSelection_B;
class UMainMenu_B;
class UCharacterSelectionComponent_B;

UCLASS()
class BRAWLINN_API AMenuGameMode_B : public AGameMode_B
{
	GENERATED_BODY()
	
public:

	AMenuGameMode_B();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UCharacterSelectionComponent_B* CharacterSelectionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMainMenu_B> BP_MainMenu;

	UMainMenu_B* MainMenuWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UCharacterSelection_B> BP_CharacterSelection;

	UCharacterSelection_B* CharacterSelection = nullptr;

	virtual void UpdateViewTarget(APlayerController_B* PlayerController) override;

	UFUNCTION(BlueprintCallable)
	void UpdateViewTargets();

	UFUNCTION(BlueprintCallable)
	void ShowMainMenu();


	UFUNCTION(BlueprintCallable)
		void HideMainMenu();

	UFUNCTION()
	void LS_IntroFinished();

	UFUNCTION()
	void LS_QuitGame();

	UFUNCTION()
	void LS_PlayGame();

	UFUNCTION()
		void LS_ToSelectionFinished();

	void UpdateNumberOfActivePlayers();

	void StartGame();


	// ** Level Sequence **
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		ULevelSequence* LS_Intro = nullptr;

	ALevelSequenceActor* LSA_Intro = nullptr;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		ULevelSequence* LS_MainMenu = nullptr;

	ALevelSequenceActor* LSA_MainMenu = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		ULevelSequence* LS_ToSelection = nullptr;

	ALevelSequenceActor* LSA_ToSelection = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		ULevelSequence* LS_Selection = nullptr;

	UPROPERTY(BlueprintReadWrite)
	ALevelSequenceActor* LSA_Selection = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ACameraActor* Camera = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACameraActor> BP_SelectionCamera; 

	class ACameraActor* SelectionCamera = nullptr;

	bool bIsQuitting = false;

	// ** Ready up **

public: 

	int PlayersActive = 0;
	int PlayersReady = 0;

};
