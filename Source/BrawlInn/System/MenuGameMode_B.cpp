// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameMode_B.h"
#include "BrawlInn.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "Player/PlayerController_B.h"
#include "Camera/CameraActor.h"
#include "Blueprint/UserWidget.h"
#include "UI/Menus/MainMenu_B.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "MovieSceneSequencePlayer.h"

void AMenuGameMode_B::BeginPlay()
{
	Super::BeginPlay();

	SpawnCharacter_D.AddDynamic(this, &AMenuGameMode_B::UpdateViewTarget);
	DespawnCharacter_D.AddDynamic(this, &AMenuGameMode_B::UpdateViewTarget);

	
	/// Level sequence stuff
	FMovieSceneSequencePlaybackSettings Settings;
	ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LS_Intro, Settings, LSA_Intro);

	ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LS_Selection, Settings, LSA_Selection);

	LSA_Intro->GetSequencePlayer()->Play();
	LSA_Intro->GetSequencePlayer()->OnFinished.AddDynamic(this, &AMenuGameMode_B::LS_IntroFinished);


}

void AMenuGameMode_B::UpdateViewTarget(APlayerController_B* PlayerController) //TODO TASK: #38
{
	if (IsValid(Camera))
		PlayerController->SetViewTargetWithBlend(Camera);
}

void AMenuGameMode_B::UpdateViewTargets()
{
	for (const auto& PlayerController : PlayerControllers)
	{
		if (IsValid(Camera))
			PlayerController->SetViewTargetWithBlend(Camera);
	}
}

void AMenuGameMode_B::ShowMainMenu()
{
	if (!PlayerControllers.IsValidIndex(0))
		return;

	MainMenuWidget = CreateWidget<UMainMenu_B>(PlayerControllers[0], BP_MainMenu);
	if (!MainMenuWidget)
		return; 

	MainMenuWidget->AddToViewport();

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(MainMenuWidget->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerControllers[0]->SetInputMode(InputModeData);
	
	PlayerControllers[0]->bShowMouseCursor = true;
}

void AMenuGameMode_B::HideMainMenu()
{
	MainMenuWidget->RemoveFromParent();

	PlayerControllers[0]->SetInputMode(FInputModeGameOnly());
	PlayerControllers[0]->bShowMouseCursor = false;
}

void AMenuGameMode_B::LS_IntroFinished()
{
	if (bIsQuitting)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
		return;
	}

	FMovieSceneSequencePlaybackSettings Settings;
	ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LS_MainMenu, Settings, LSA_MainMenu);

	LSA_MainMenu->GetSequencePlayer()->PlayLooping();
	ShowMainMenu();
}

void AMenuGameMode_B::LS_QuitGame()
{
	LSA_MainMenu->GetSequencePlayer()->Stop();
	LSA_Intro->GetSequencePlayer()->PlayReverse();
	bIsQuitting = true;
}

void AMenuGameMode_B::LS_PlayGame()
{
	LSA_MainMenu->GetSequencePlayer()->Stop();
	FMovieSceneSequencePlaybackSettings Settings;
	ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LS_ToSelection, Settings, LSA_ToSelection);

	LSA_ToSelection->GetSequencePlayer()->Play();
	
	LSA_ToSelection->GetSequencePlayer()->OnFinished.AddDynamic(this, &AMenuGameMode_B::LS_ToSelectionFinished);

}

void AMenuGameMode_B::LS_ToSelectionFinished()
{
	
	LSA_Selection->GetSequencePlayer()->PlayLooping();
}