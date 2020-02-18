// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuPlayerController_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "System/GameModes/MenuGameMode_B.h"
#include "Characters/Player/SelectionPawn_B.h"
//#include "Characters/Player/PlayerCharacter_B.h"

void AMenuPlayerController_B::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent != nullptr)
	{
		InputComponent->BindAction("Select", IE_Pressed, this, &AMenuPlayerController_B::TrySelect);
		InputComponent->BindAction("SelectLeft", IE_Pressed, this, &AMenuPlayerController_B::SelectLeft);
		InputComponent->BindAction("SelectRight", IE_Pressed, this, &AMenuPlayerController_B::SelectRight);
		InputComponent->BindAction("Unselect", IE_Pressed, this, &AMenuPlayerController_B::Unselect);
	}
}

ASelectionPawn_B* AMenuPlayerController_B::GetSelectionPawn()
{
	if (!IsValid(SelectionPawn))
		SelectionPawn = Cast<ASelectionPawn_B>(GetPawn());
	return SelectionPawn;
}

int AMenuPlayerController_B::GetSelectedCharacterIndex() const
{
	return SelectedCharacterIndex;
}

void AMenuPlayerController_B::SetSelectedCharacterIndex(int Value)
{
	SelectedCharacterIndex = Value;
}

void AMenuPlayerController_B::TrySelect()
{
	if (PlayerCharacter)
		return;

	AMenuGameMode_B* GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
		GameMode->Select(this, SelectedCharacterIndex);
}

void AMenuPlayerController_B::Unselect()
{
	if (!PlayerCharacter)
		return;

	AMenuGameMode_B* GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->UnSelect(this);
		GameMode->Hover(this, SelectedCharacterIndex);
	}
}

void AMenuPlayerController_B::SelectLeft()
{
	if (PlayerCharacter)
		return;

	AMenuGameMode_B* GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
		GameMode->HoverLeft(this);
}

void AMenuPlayerController_B::SelectRight()
{
	if (PlayerCharacter)
		return;

	AMenuGameMode_B* GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
		GameMode->HoverRight(this);
}
