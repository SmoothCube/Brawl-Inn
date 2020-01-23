// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameMode_B.h"
#include "BrawlInn.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "Player/PlayerController_B.h"
#include "Camera/CameraActor.h"
#include "Blueprint/UserWidget.h"
#include "UI/Menus/CharacterSelection_B.h"
#include "UI/Menus/MainMenu_B.h"
#include "Player/PlayerCharacter_B.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "MovieSceneSequencePlayer.h"
#include "Components/CharacterSelectionComponent_B.h"

AMenuGameMode_B::AMenuGameMode_B()
{
	CharacterSelectionComponent = CreateDefaultSubobject<UCharacterSelectionComponent_B>("Character Selection");
}

void AMenuGameMode_B::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(true);

	CharacterSelectionComponent->CharacterSelected_D.BindUObject(this, &AMenuGameMode_B::UpdateNumberOfActivePlayers);
	CharacterSelectionComponent->CharacterUnselected_D.BindUObject(this, &AMenuGameMode_B::UpdateNumberOfActivePlayers);

	/// Level sequence stuff
	FMovieSceneSequencePlaybackSettings Settings;
	ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LS_Intro, Settings, LSA_Intro);
	ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LS_MainMenu, Settings, LSA_MainMenu);
	ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LS_Selection, Settings, LSA_Selection);
	ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LS_ToSelection, Settings, LSA_ToSelection);

	CharacterSelection = CreateWidget<UCharacterSelection_B>(PlayerControllers[0], BP_CharacterSelection);

	// Find Character selection camera
	TArray<AActor*> Cameras;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_SelectionCamera, Cameras);
	SelectionCamera = Cast<ACameraActor>(Cameras[0]);

	LSA_Intro->GetSequencePlayer()->Play();
	LSA_Intro->GetSequencePlayer()->OnFinished.AddDynamic(this, &AMenuGameMode_B::LS_IntroFinished);
}

void AMenuGameMode_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MainMenuWidget || !IsValid(MainMenuWidget))
		return;

	MainMenuWidget->MenuTick();
}


void AMenuGameMode_B::UpdateViewTarget(APlayerController_B* PlayerController)
{
	if (IsValid(SelectionCamera))
		PlayerController->SetViewTargetWithBlend(SelectionCamera);
}

void AMenuGameMode_B::UpdateViewTargets() // Used for sequences
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

	PlayerControllers[0]->SetInputMode(InputModeData);
}

void AMenuGameMode_B::HideMainMenu()
{
	MainMenuWidget->RemoveFromParent();

	PlayerControllers[0]->SetInputMode(FInputModeGameOnly());
}

void AMenuGameMode_B::LS_IntroFinished()
{
	if (bIsQuitting)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
		return;
	}

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

	LSA_ToSelection->GetSequencePlayer()->Play();

	LSA_ToSelection->GetSequencePlayer()->OnFinished.AddDynamic(this, &AMenuGameMode_B::LS_ToSelectionFinished);
}

void AMenuGameMode_B::LS_ToSelectionFinished()
{

	if (!CharacterSelection)
		return;

	CharacterSelection->AddToViewport();

	for (const auto& PlayerController : PlayerControllers)
	{
		if (IsValid(SelectionCamera))
			PlayerController->SetViewTargetWithBlend(SelectionCamera);
	}
}

void AMenuGameMode_B::UpdateNumberOfActivePlayers()
{
	int NumberOfPlayers = 0;

	for (const auto& Controller : PlayerControllers)
	{
		if (Cast<APlayerController_B>(Controller)->HasValidCharacter())
			NumberOfPlayers++;
	}
	PlayersActive = NumberOfPlayers;
	CharacterSelection->UpdateNumberOfPlayersText(PlayersActive);
}

void AMenuGameMode_B::StartGame()
{
	BScreen("Start");

	UGameplayStatics::OpenLevel(GetWorld(), "Graybox_V3");
}