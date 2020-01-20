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

UCLASS()
class BRAWLINN_API AMenuGameMode_B : public AGameMode_B
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACameraActor* Camera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMainMenu_B> BP_MainMenu;

	UMainMenu_B* MainMenuWidget = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UCharacterSelection_B> BP_CharacterSelection;

	UCharacterSelection_B* CharacterSelection = nullptr;


	UFUNCTION()
	void UpdateViewTarget(APlayerController_B* PlayerController);

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

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGameCamera_B> BP_GameCamera; //TODO Temporary 

	class AGameCamera_B* GameCamera = nullptr;

	bool bIsQuitting = false;


};
