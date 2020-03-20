// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuGameMode_B.h"
#include "Kismet/GameplayStatics.h"
#include "PaperSpriteComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "BrawlInn.h"
#include "System/Camera/GameCamera_B.h"
#include "UI/Menus/CharacterSelection_B.h"
#include "UI/Menus/MainMenu_B.h"
#include "Characters/Player/SelectionPawn_B.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Characters/Player/MenuPlayerController_B.h"

void AMenuGameMode_B::BeginPlay()
{
	Super::BeginPlay();
}

void AMenuGameMode_B::PostLevelLoad_Implementation()
{
	SetActorTickEnabled(true);

	GameCamera = GetWorld()->SpawnActor<AGameCamera_B>(BP_GameCamera, FTransform());
	GameCamera->SetActorRotation(FRotator(0, -65, 0));

	CharacterSelectionWidget = CreateWidget<UCharacterSelection_B>(GetWorld(), BP_CharacterSelectionOverlay);

	for (auto Controller : PlayerControllers)
		MenuPlayerControllers.Add(Cast<AMenuPlayerController_B>(Controller));

	TArray<AActor*> TCharacters;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerCharacter_B::StaticClass(), FName("Selection"), TCharacters);

	if (TCharacters.Num() == 0)
		BError("Can't find any characters to select!");

	for (auto Character : TCharacters)
	{
		APlayerCharacter_B* PlayerCharacter = Cast<APlayerCharacter_B>(Character);
		Characters.Add(PlayerCharacter);
		PlayerCharacter->GameCamera = GameCamera;
		AddCameraFocusPoint(PlayerCharacter);
		PlayerCharacter->MakeInvulnerable(-1, false);
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
		auto SelectionPawn = MenuPlayerControllers[i]->GetSelectionPawn();

		if (SelectionIndicators.IsValidIndex(i))
			SelectionPawn->GetSpriteIcon()->SetSprite(SelectionIndicators[i]);
		if (CharacterVariants.IsValidIndex(i))
			SelectionPawn->GetSpriteIcon()->SetSpriteColor(CharacterVariants[i].TextColor);

		SelectionPawn->SetActorLocation(Characters[i]->GetActorLocation() + SelectionIndicatorOffsetLocation);

		MenuPlayerControllers[i]->SetCharacterVariantIndex(i);
	}

	UpdateViewTargets();

	ShowMainMenu();
}

void AMenuGameMode_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MainMenuWidget && IsValid(MainMenuWidget))
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
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);

	PlayerControllers[0]->SetInputMode(InputModeData);
}

void AMenuGameMode_B::HideMainMenu()
{
	MainMenuWidget->RemoveFromParent();

	PlayerControllers[0]->SetInputMode(FInputModeGameOnly());
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

void AMenuGameMode_B::PlayButtonClicked_Implementation()
{
	HideMainMenu();

	if (CharacterSelectionWidget)
		CharacterSelectionWidget->AddToViewport();
}

void AMenuGameMode_B::Select(AMenuPlayerController_B* PlayerControllerThatSelected, const int Index)
{
	PlayersActive++;
	UpdateNumberOfActivePlayers();
	UpdateNumberOfReadyPlayers();
	if (Characters.IsValidIndex(Index))
	{
		PlayerControllerThatSelected->GetSelectionPawn()->Destroy();
		FPlayerInfo Info = PlayerControllerThatSelected->GetPlayerInfo();
		FCharacterVariants& Variant = CharacterVariants[PlayerControllerThatSelected->GetCharacterVariantIndex()];
		Variant.bTaken = true;
		Info.CharacterVariant = Variant;
		Info.ID = UGameplayStatics::GetPlayerControllerID(PlayerControllerThatSelected);
		PlayerControllerThatSelected->SetPlayerInfo(Info);
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

	const int ControllerID = UGameplayStatics::GetPlayerControllerID(PlayerControllerThatSelected);

	Character->GetMovementComponent()->StopMovementImmediately();
	Character->SetActorTransform(CharacterStartTransforms[ControllerID]);

	Characters.Sort([](const APlayerCharacter_B& Left, const APlayerCharacter_B& Right)
		{
			return Left.GetActorLocation().Y < Right.GetActorLocation().Y;
		});

	const int VariantIndex = PlayerControllerThatSelected->GetCharacterVariantIndex();
	FPlayerInfo Info = PlayerControllerThatSelected->GetPlayerInfo();
	CharacterVariants[VariantIndex].bTaken = false;
	Info.CharacterVariant = FCharacterVariants();
	PlayerControllerThatSelected->SetPlayerInfo(Info);

	ASelectionPawn_B* SelectionPawn = GetWorld()->SpawnActor<ASelectionPawn_B>(BP_SelectionPawn, CharacterStartTransforms[ControllerID]);
	if (SelectionIndicators.IsValidIndex(ControllerID))
		SelectionPawn->GetSpriteIcon()->SetSprite(SelectionIndicators[ControllerID]);
	if (CharacterVariants.IsValidIndex(VariantIndex))
		SelectionPawn->GetSpriteIcon()->SetSpriteColor(CharacterVariants[VariantIndex].TextColor);

	PlayerControllerThatSelected->Possess(SelectionPawn);
	SelectionPawn->SetActorLocation(Characters[ControllerID]->GetActorLocation() + SelectionIndicatorOffsetLocation);
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
	const int ID = UGameplayStatics::GetPlayerControllerID(PlayerController);
	PlayerController->SetCharacterVariantIndex((PlayerController->GetCharacterVariantIndex() ? PlayerController->GetCharacterVariantIndex() : CharacterVariants.Num()) - 1);

	FCharacterVariants Variant = CharacterVariants[PlayerController->GetCharacterVariantIndex()];
	while (Variant.bTaken == true)
	{
		PlayerController->SetCharacterVariantIndex((PlayerController->GetCharacterVariantIndex() ? PlayerController->GetCharacterVariantIndex() : CharacterVariants.Num()) - 1);
		Variant = CharacterVariants[PlayerController->GetCharacterVariantIndex()];
	}

	UpdateCharacterVisuals(PlayerController, SelectionPawn, ID);
}

void AMenuGameMode_B::HoverRight(AMenuPlayerController_B* PlayerController)
{
	const auto SelectionPawn = PlayerController->GetSelectionPawn();
	const int ID = UGameplayStatics::GetPlayerControllerID(PlayerController);
	PlayerController->SetCharacterVariantIndex((PlayerController->GetCharacterVariantIndex() + 1) % CharacterVariants.Num()); // Set the next index as current index.

	FCharacterVariants Variant = CharacterVariants[PlayerController->GetCharacterVariantIndex()];
	while (Variant.bTaken == true)
	{
		PlayerController->SetCharacterVariantIndex((PlayerController->GetCharacterVariantIndex() + 1) % CharacterVariants.Num());
		Variant = CharacterVariants[PlayerController->GetCharacterVariantIndex()];
	}

	UpdateCharacterVisuals(PlayerController, SelectionPawn, ID);
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
