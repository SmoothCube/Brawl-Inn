// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuGameMode_B.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSequencePlayer.h"
#include "LevelSequence.h"
#include "PaperSpriteComponent.h"
#include "Camera/CameraActor.h"

#include "BrawlInn.h"
#include "Characters/Player/MenuPlayerController_B.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Characters/Player/SelectionPawn_B.h"
#include "System/Camera/GameCamera_B.h"
#include "System/GameInstance_B.h"
#include "UI/Widgets/MainMenu_B.h"
#include "UI/Widgets/CharacterSelectionOverlay_B.h"

AMenuGameMode_B::AMenuGameMode_B()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMenuGameMode_B::PostLevelLoad()
{
	GameCamera = GetWorld()->SpawnActor<AGameCamera_B>(BP_GameCamera, FTransform());
	GameCamera->SetActorRotation(FRotator(0, -65, 0));
	ACameraActor* IntroCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), IntroCamera_BP));
	UpdateViewTargets(IntroCamera);

	for (auto Controller : PlayerControllers)
		MenuPlayerControllers.Add(Cast<AMenuPlayerController_B>(Controller));

	PrepareCharacterSelection();

	ShowMainMenu();
}

void AMenuGameMode_B::PrepareCharacterSelection()
{
	TArray<AActor*> SelectionCharacters;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerCharacter_B::StaticClass(), FName("Selection"), SelectionCharacters);

	checkf(SelectionCharacters.Num() != 0, TEXT("Can't find any characters to select!"));

	for (AActor* Character : SelectionCharacters)
	{
		APlayerCharacter_B* PlayerCharacter = Cast<APlayerCharacter_B>(Character);
		PlayerCharacter->SetGameCamera(GameCamera);
		PlayerCharacter->MakeInvulnerable(-1, false);
		Characters.Add(PlayerCharacter);
	}

	Characters.Sort([](const APlayerCharacter_B& Left, const APlayerCharacter_B& Right)
		{
			return Left.GetActorLocation().Y < Right.GetActorLocation().Y;
		});

	CharacterStartTransforms.AddZeroed(Characters.Num());

	for (int i = 0; i < Characters.Num(); i++)
		CharacterStartTransforms[i] = Characters[i]->GetActorTransform();

	for (int i = 0; i < MenuPlayerControllers.Num(); i++)
	{
		checkf(SelectionIndicators.IsValidIndex(i), TEXT("Selection indicator index %i is not valid"), i);
		checkf(CharacterVariants.IsValidIndex(i), TEXT("Character variant index %i is not valid"), i);

		ASelectionPawn_B* SelectionPawn = MenuPlayerControllers[i]->GetSelectionPawn();

		SelectionPawn->GetSpriteIcon()->SetSprite(SelectionIndicators[i]);

		SelectionPawn->GetSpriteIcon()->SetSpriteColor(CharacterVariants[i].TextColor);

		SelectionPawn->SetActorLocation(Characters[i]->GetActorLocation() + SelectionIndicatorOffsetLocation);

		MenuPlayerControllers[i]->SetCharacterVariantIndex(i);
	}
}

void AMenuGameMode_B::ShowMainMenu()
{
	check(PlayerControllers.IsValidIndex(0));
	check(IsValid(BP_MainMenu));

	MainMenuWidget = CreateWidget<UMainMenu_B>(PlayerControllers[0], BP_MainMenu);

	check(IsValid(MainMenuWidget));

	MainMenuWidget->AddToViewport();

	FInputModeUIOnly InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);

	PlayerControllers[0]->SetInputMode(InputModeData);
}

void AMenuGameMode_B::HideMainMenu()
{
	check(IsValid(MainMenuWidget));
	check(PlayerControllers.IsValidIndex(0));

	MainMenuWidget->RemoveFromParent();

	PlayerControllers[0]->SetInputMode(FInputModeGameOnly());
}

void AMenuGameMode_B::UpdateCharacterSelectionOverlay()
{
	CharacterSelectionOverlay->UpdateNumberOfPlayersReady(PlayersReady);
}

int AMenuGameMode_B::GetPlayersActive() const
{
	return PlayersActive;
}

void AMenuGameMode_B::SetPlayersActive(const int Value)
{
	PlayersActive = Value;
}

int AMenuGameMode_B::GetPlayersReady() const
{
	return PlayersReady;
}

void AMenuGameMode_B::SetPlayersReady(const int Value)
{
	PlayersReady = Value;
}

void AMenuGameMode_B::OnMenuPlayButtonClicked()
{
	DisableControllerInputs();
	HideMainMenu();

	FMovieSceneSequencePlaybackSettings Settings;
	Settings.bPauseAtEnd = true;
	ALevelSequenceActor* OutActor;

	ULevelSequencePlayer* Player = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), IntroLevelSequence, Settings, OutActor);
	Player->Play();
	Player->OnPause.AddDynamic(this, &AMenuGameMode_B::OnIntroLevelSequencePaused);
}

void AMenuGameMode_B::PrepareGameStart()
{
	FMovieSceneSequencePlaybackSettings Settings;
	Settings.bPauseAtEnd = true;
	ALevelSequenceActor* OutActor;

	checkf(ToGameLevelSequence, TEXT("ToGameLevelSequence is not set! Make sure to set it in the blueprint"));
	ULevelSequencePlayer* Player = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), ToGameLevelSequence, Settings, OutActor);
	
	Player->Play();
	Player->OnPause.AddDynamic(this, &AMenuGameMode_B::OnToGameLevelSequencePaused);
	
}

void AMenuGameMode_B::OnToGameLevelSequencePaused()
{
	ACameraActor* SequenceCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), SequenceCamera_BP));
	check(IsValid(SequenceCamera));
	check(IsValid(GameInstance));
	
	GameInstance->SetCameraSwapTransform(SequenceCamera->GetActorTransform());
	UGameplayStatics::OpenLevel(GetWorld(), *GameInstance->GetGameMapName());
}

void AMenuGameMode_B::OnIntroLevelSequencePaused()
{
	ACameraActor* SequenceCamera =  Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), SequenceCamera_BP));
	check(IsValid(SequenceCamera));
	UpdateViewTargets(SequenceCamera, 1, true);

	EnableControllerInputs();

	CharacterSelectionOverlay = CreateWidget<UCharacterSelectionOverlay_B>(GetWorld(), BP_CharacterSelectionOverlay);
	check(IsValid(CharacterSelectionOverlay));
	CharacterSelectionOverlay->AddToViewport();
	
}


void AMenuGameMode_B::Select(AMenuPlayerController_B* PlayerControllerThatSelected, const int Index)
{
	PlayersActive++;

	const int PlayerControllerID = UGameplayStatics::GetPlayerControllerID(PlayerControllerThatSelected);
	FPlayerInfo Info = PlayerControllerThatSelected->GetPlayerInfo();

	FCharacterVariants& Variant = CharacterVariants[PlayerControllerThatSelected->GetCharacterVariantIndex()];
	Variant.bTaken = true;

	Info.CharacterVariant = Variant;
	Info.ID = PlayerControllerID;

	PlayerControllerThatSelected->GetSelectionPawn()->SetActorHiddenInGame(true);
	PlayerControllerThatSelected->SetPlayerInfo(Info);
	PlayerControllerThatSelected->Possess(Characters[PlayerControllerID]);

	if (PlayersActiveUpdated.IsBound())
		PlayersActiveUpdated.Execute();

	UpdateOtherSelections();
}

void AMenuGameMode_B::UnSelect(AMenuPlayerController_B* PlayerControllerThatSelected)
{
	PlayersActive--;

	APlayerCharacter_B* Character = PlayerControllerThatSelected->GetPlayerCharacter();
	PlayerControllerThatSelected->UnPossess();

	const int PlayerControllerID = UGameplayStatics::GetPlayerControllerID(PlayerControllerThatSelected);

	Character->SetActorTransform(CharacterStartTransforms[PlayerControllerID]);
	Character->GetMovementComponent()->StopMovementImmediately();

	/// Clear the PlayerController's CharacterVariant
	FPlayerInfo Info = PlayerControllerThatSelected->GetPlayerInfo();
	Info.CharacterVariant = FCharacterVariants();
	PlayerControllerThatSelected->SetPlayerInfo(Info);

	CharacterVariants[PlayerControllerThatSelected->GetCharacterVariantIndex()].bTaken = false;

	PlayerControllerThatSelected->Possess(PlayerControllerThatSelected->GetSelectionPawn());
	PlayerControllerThatSelected->GetSelectionPawn()->SetActorHiddenInGame(false);

	if (PlayersActiveUpdated.IsBound())
		PlayersActiveUpdated.Execute();
}

void AMenuGameMode_B::UpdateCharacterVisuals(AMenuPlayerController_B* PlayerController, ASelectionPawn_B* const SelectionPawn, const int ID)
{
	Characters[ID]->GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(1, CharacterVariants[PlayerController->GetCharacterVariantIndex()].MeshMaterial);
	Characters[ID]->GetDirectionIndicatorPlane()->CreateAndSetMaterialInstanceDynamicFromMaterial(0, CharacterVariants[PlayerController->GetCharacterVariantIndex()].IndicatorMaterial);
	SelectionPawn->GetSpriteIcon()->SetSpriteColor(CharacterVariants[PlayerController->GetCharacterVariantIndex()].TextColor);
}

void AMenuGameMode_B::HoverLeft(AMenuPlayerController_B* PlayerController)
{
	const auto SelectionPawn = PlayerController->GetSelectionPawn();
	const int PlayerControllerID = UGameplayStatics::GetPlayerControllerID(PlayerController);
	PlayerController->SetCharacterVariantIndex((PlayerController->GetCharacterVariantIndex() ? PlayerController->GetCharacterVariantIndex() : CharacterVariants.Num()) - 1);

	FCharacterVariants Variant = CharacterVariants[PlayerController->GetCharacterVariantIndex()];
	while (Variant.bTaken == true)
	{
		PlayerController->SetCharacterVariantIndex((PlayerController->GetCharacterVariantIndex() ? PlayerController->GetCharacterVariantIndex() : CharacterVariants.Num()) - 1);
		Variant = CharacterVariants[PlayerController->GetCharacterVariantIndex()];
	}

	UpdateCharacterVisuals(PlayerController, SelectionPawn, PlayerControllerID);
}

void AMenuGameMode_B::HoverRight(AMenuPlayerController_B* PlayerController)
{
	ASelectionPawn_B* SelectionPawn = PlayerController->GetSelectionPawn();

	const int PlayerControllerID = UGameplayStatics::GetPlayerControllerID(PlayerController);

	PlayerController->SetCharacterVariantIndex((PlayerController->GetCharacterVariantIndex() + 1) % CharacterVariants.Num()); // Set the next index as current index.

	FCharacterVariants Variant = CharacterVariants[PlayerController->GetCharacterVariantIndex()];
	while (Variant.bTaken == true)
	{
		PlayerController->SetCharacterVariantIndex((PlayerController->GetCharacterVariantIndex() + 1) % CharacterVariants.Num());
		Variant = CharacterVariants[PlayerController->GetCharacterVariantIndex()];
	}

	UpdateCharacterVisuals(PlayerController, SelectionPawn, PlayerControllerID);
}

UCharacterSelectionOverlay_B* AMenuGameMode_B::GetCharacterSelectionOverlay() const
{
	return CharacterSelectionOverlay;
}

void AMenuGameMode_B::UpdateOtherSelections()
{
	for (int j = 0; j < MenuPlayerControllers.Num(); j++)
	{
		const auto Controller = MenuPlayerControllers[j];
		if (Controller->GetPlayerCharacter())
			continue;

		for (int i = 0; i < CharacterVariants.Num(); i++)
		{
			if (CharacterVariants[i].bTaken == true)
			{
				if (Controller->GetCharacterVariantIndex() == i)
					HoverRight(Controller);
			}
		}
	}
}
