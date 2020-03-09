// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuGameMode_B.h"
#include "MovieSceneSequencePlayer.h"
#include "LevelSequencePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "LevelSequenceActor.h"
#include "PaperSpriteComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


#include "BrawlInn.h"
#include "UI/Menus/CharacterSelection_B.h"
#include "UI/Menus/MainMenu_B.h"
#include "Characters/Player/SelectionPawn_B.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Characters/Player/MenuPlayerController_B.h"

void AMenuGameMode_B::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(true);

	/// Level sequence stuff
	const FMovieSceneSequencePlaybackSettings Settings;
	ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LS_Intro, Settings, LSA_Intro);
	ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LS_MainMenu, Settings, LSA_MainMenu);
	ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LS_Selection, Settings, LSA_Selection);
	ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LS_ToSelection, Settings, LSA_ToSelection);

	CharacterSelectionWidget = CreateWidget<UCharacterSelection_B>(GetWorld(), BP_CharacterSelection);

	for (auto Controller : PlayerControllers)
		MenuPlayerControllers.Add(Cast<AMenuPlayerController_B>(Controller));

	TArray<AActor*> TCharacters;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerCharacter_B::StaticClass(), FName("Selection"), TCharacters);

	if (TCharacters.Num() == 0)
		BError("Can't find any characters to select!");

	for (const auto& Character : TCharacters)
	{
		APlayerCharacter_B* PlayerCharacter = Cast<APlayerCharacter_B>(Character);
		Characters.Add(PlayerCharacter);
		PlayerCharacter->MakeInvulnerable(-1, false);

	}

	Characters.Sort([](const APlayerCharacter_B& Left, const APlayerCharacter_B& Right) {
		return Left.GetActorLocation().Y < Right.GetActorLocation().Y;
		});

	CharacterIsInLine.AddZeroed(Characters.Num());
	CharacterStartTransforms.AddZeroed(Characters.Num());

	for (int i = 0; i < Characters.Num(); i++)
	{
		CharacterIsInLine[i] = false;
		CharacterStartTransforms[i] = Characters[i]->GetActorTransform();
	}
	// Find Character selection camera
	TArray<AActor*> Cameras;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BP_SelectionCamera, Cameras);
	SelectionCamera = Cast<ACameraActor>(Cameras[0]);

	for (int i = 0; i < MenuPlayerControllers.Num(); i++)
	{
		auto SelectionPawn = MenuPlayerControllers[i]->GetSelectionPawn();
		FVector Offset = SelectionArrowSpacing * (i);
		if (SelectionIndicators.IsValidIndex(i))
			SelectionPawn->GetSpriteIcon()->SetSprite(SelectionIndicators[i]);
		if (CharacterVariants.IsValidIndex(i))
			SelectionPawn->GetSpriteIcon()->SetSpriteColor(CharacterVariants[i].TextColor);
		SelectionPawn->SetActorLocation(Characters[i]->GetActorLocation() + FirstSelectionArrowLocation);

		MenuPlayerControllers[i]->SetCharacterVariantIndex(i);
	}



	LS_ToSelectionFinished(); // For å sette på mainmenu og sequencer kommenter denne linjen og uncomment de to linjene under.

	//LSA_Intro->GetSequencePlayer()->Play();
	//LSA_Intro->GetSequencePlayer()->OnFinished.AddDynamic(this, &AMenuGameMode_B::LS_IntroFinished);
}

void AMenuGameMode_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MainMenuWidget || !IsValid(MainMenuWidget))
		return;

	MainMenuWidget->MenuTick();
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

void AMenuGameMode_B::LS_PlayGame()
{
	HideMainMenu();
	LSA_MainMenu->GetSequencePlayer()->Stop();

	LSA_ToSelection->GetSequencePlayer()->Play();
	LSA_ToSelection->GetSequencePlayer()->OnFinished.AddDynamic(this, &AMenuGameMode_B::LS_ToSelectionFinished);
}

void AMenuGameMode_B::LS_QuitGame()
{
	HideMainMenu();

	LSA_MainMenu->GetSequencePlayer()->Stop();
	LSA_Intro->GetSequencePlayer()->PlayReverse();
	bIsQuitting = true;
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

void AMenuGameMode_B::LS_ToSelectionFinished()
{
	if (!CharacterSelectionWidget)
		return;

	CharacterSelectionWidget->AddToViewport();

	for (auto& PlayerController : PlayerControllers)
	{
		if (IsValid(SelectionCamera))
			PlayerController->SetViewTargetWithBlend(SelectionCamera);
	}
}

void AMenuGameMode_B::StartGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), PlayMap);
}

void AMenuGameMode_B::UpdateNumberOfActivePlayers()
{
	CharacterSelectionWidget->UpdateNumberOfPlayersText(PlayersActive);
}

void AMenuGameMode_B::UpdateNumberOfReadyPlayers()
{
	CharacterSelectionWidget->UpdateNumberOfPlayersReady(PlayersReady);
}

int AMenuGameMode_B::GetPlayersActive() const
{
	return PlayersActive;
}

void AMenuGameMode_B::SetPlayersActive(int Value)
{
	PlayersActive = Value;
}

int AMenuGameMode_B::GetPlayersReady() const
{
	return PlayersReady;
}

void AMenuGameMode_B::SetPlayersReady(int Value)
{
	PlayersReady = Value;
}

void AMenuGameMode_B::Select(AMenuPlayerController_B* PlayerControllerThatSelected, int Index)
{
	PlayersActive++;
	UpdateNumberOfActivePlayers();
	UpdateNumberOfReadyPlayers();
	if (CharacterIsInLine[Index] == false && Characters.IsValidIndex(Index))
	{
		CharacterIsInLine[Index] = true;
		PlayerControllerThatSelected->GetSelectionPawn()->Destroy();
		PlayerControllerThatSelected->Possess(Characters[UGameplayStatics::GetPlayerControllerID(PlayerControllerThatSelected)]);
	}
	UpdateOtherSelections();
}

void AMenuGameMode_B::UnSelect(AMenuPlayerController_B* PlayerControllerThatSelected)
{
	PlayersActive--;
	UpdateNumberOfActivePlayers();
	UpdateNumberOfReadyPlayers();
	APlayerCharacter_B* Character = PlayerControllerThatSelected->GetPlayerCharacter();
	PlayerControllerThatSelected->UnPossess();

	const int ID = UGameplayStatics::GetPlayerControllerID(PlayerControllerThatSelected);
	CharacterIsInLine[ID] = false;
	Character->GetMovementComponent()->StopMovementImmediately();
	Character->SetActorTransform(CharacterStartTransforms[ID]);

	Characters.Sort([](const APlayerCharacter_B& Left, const APlayerCharacter_B& Right) {
		return Left.GetActorLocation().Y < Right.GetActorLocation().Y;
		});

	ASelectionPawn_B* SelectionPawn = GetWorld()->SpawnActor<ASelectionPawn_B>(BP_SelectionPawn, CharacterStartTransforms[ID]);
	if (SelectionIndicators.IsValidIndex(ID))
		SelectionPawn->GetSpriteIcon()->SetSprite(SelectionIndicators[ID]);
	if (CharacterVariants.IsValidIndex(ID))
		SelectionPawn->GetSpriteIcon()->SetSpriteColor(CharacterVariants[ID].TextColor);
	PlayerControllerThatSelected->Possess(SelectionPawn);
}

void AMenuGameMode_B::HoverLeft(AMenuPlayerController_B* PlayerController)
{
	const auto SelectionPawn = PlayerController->GetSelectionPawn();
	const int ID = UGameplayStatics::GetPlayerControllerID(PlayerController);
	PlayerController->SetCharacterVariantIndex((PlayerController->GetCharacterVariantIndex() ? PlayerController->GetCharacterVariantIndex() : CharacterVariants.Num()) - 1);
	while (CharacterIsInLine[PlayerController->GetCharacterVariantIndex()])
		PlayerController->SetCharacterVariantIndex((PlayerController->GetCharacterVariantIndex() ? PlayerController->GetCharacterVariantIndex() : CharacterVariants.Num()) - 1);

	Characters[ID]->GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(1, CharacterVariants[PlayerController->GetCharacterVariantIndex()].MeshMaterial);
	Characters[ID]->GetDirectionIndicatorPlane()->CreateAndSetMaterialInstanceDynamicFromMaterial(0, CharacterVariants[PlayerController->GetCharacterVariantIndex()].IndicatorMaterial);
	SelectionPawn->GetSpriteIcon()->SetSpriteColor(CharacterVariants[PlayerController->GetCharacterVariantIndex()].TextColor);

	
	//const FVector Offset = SelectionArrowSpacing * (MenuPlayerControllers.Find(PlayerController));
	//SelectionPawn->SetActorLocation(Characters[PlayerController->GetSelectedCharacterIndex()]->GetActorLocation() + FirstSelectionArrowLocation + Offset);
}

void AMenuGameMode_B::Hover(AMenuPlayerController_B* PlayerController, const int Index)
{
	auto SelectionPawn = PlayerController->GetSelectionPawn();
	PlayerController->SetCharacterVariantIndex(Index); // Set the next index as current index.

	SelectionPawn->SetActorLocation(Characters[UGameplayStatics::GetPlayerControllerID(PlayerController)]->GetActorLocation() + FirstSelectionArrowLocation);
}

void AMenuGameMode_B::HoverRight(AMenuPlayerController_B* PlayerController)
{
	const auto SelectionPawn = PlayerController->GetSelectionPawn();
	const int ID = UGameplayStatics::GetPlayerControllerID(PlayerController);
	PlayerController->SetCharacterVariantIndex((PlayerController->GetCharacterVariantIndex() + 1) % CharacterVariants.Num()); // Set the next index as current index.
	while (CharacterIsInLine[PlayerController->GetCharacterVariantIndex()]) // Loop until you find a valid index and set that index to the current.
		PlayerController->SetCharacterVariantIndex((PlayerController->GetCharacterVariantIndex() + 1) % CharacterVariants.Num());

	Characters[ID]->GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(1, CharacterVariants[PlayerController->GetCharacterVariantIndex()].MeshMaterial);
	Characters[ID]->GetDirectionIndicatorPlane()->CreateAndSetMaterialInstanceDynamicFromMaterial(0, CharacterVariants[PlayerController->GetCharacterVariantIndex()].IndicatorMaterial);
	SelectionPawn->GetSpriteIcon()->SetSpriteColor(CharacterVariants[PlayerController->GetCharacterVariantIndex()].TextColor);
}

void AMenuGameMode_B::UpdateOtherSelections()
{
	for (int j = 0; j < MenuPlayerControllers.Num(); j++)
	{
		const auto Controller = MenuPlayerControllers[j];
		if (Controller->GetPlayerCharacter())
			continue;

		for (int i = 0; i < CharacterIsInLine.Num(); i++)
		{
			if (CharacterIsInLine[i] == true)
			{
				if (Controller->GetCharacterVariantIndex() == i)
				{
					HoverRight(Controller);
				}
			}
		}
	}
}

void AMenuGameMode_B::UpdateViewTarget(AGamePlayerController_B* PlayerController)
{
	if (IsValid(SelectionCamera))
		PlayerController->SetViewTargetWithBlend(SelectionCamera);
}

void AMenuGameMode_B::UpdateViewTargets() // Used for sequences
{
	for (auto& PlayerController : PlayerControllers)
	{
		if (IsValid(Camera))
			PlayerController->SetViewTargetWithBlend(Camera);
	}
}