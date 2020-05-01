// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuPlayerController_B.h"

#include "BrawlInn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "System/GameModes/MenuGameMode_B.h"
#include "Characters/Player/SelectionPawn_B.h"
#include "Characters/Player/PlayerCharacter_B.h"

void AMenuPlayerController_B::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PlayerCharacter = Cast<APlayerCharacter_B>(InPawn);
	if (PlayerCharacter)
	{
		PlayerCharacter->MakeVulnerable();
	}
}

void AMenuPlayerController_B::OnUnPossess()
{
	APlayerCharacter_B* PC = PlayerCharacter;
	Super::OnUnPossess();
	
	if (PC)
	{
		PC->GetMovementComponent()->StopMovementImmediately();
		PC->MakeInvulnerable(-1, false);
	}
}

ASelectionPawn_B* AMenuPlayerController_B::GetSelectionPawn()
{
	if (!IsValid(SelectionPawn))
		SelectionPawn = Cast<ASelectionPawn_B>(GetPawn());
	return SelectionPawn;
}

int AMenuPlayerController_B::GetCharacterVariantIndex() const
{
	return CharacterVariantIndex;
}

void AMenuPlayerController_B::SetCharacterVariantIndex(const int Value)
{
	CharacterVariantIndex = Value;
}

void AMenuPlayerController_B::LeftStickRightPressed()
{
	if (bIsInMenuMode)
		return;
	
	SelectRight();
}

void AMenuPlayerController_B::LeftStickLeftPressed()
{
	if (bIsInMenuMode)
		return;
	
	SelectLeft();
}

void AMenuPlayerController_B::LeftShoulderPressed()
{
	if (bIsInMenuMode)
		return;
	
	SelectLeft();
	Super::LeftShoulderPressed();
}

void AMenuPlayerController_B::DPadLeftPressed()
{
	if (bIsInMenuMode)
		return;
	
	SelectLeft();
}

void AMenuPlayerController_B::DPadRightPressed()
{
	if (bIsInMenuMode)
		return;
	
	SelectRight();
}

void AMenuPlayerController_B::RightShoulderPressed()
{
	if (bIsInMenuMode)
		return;
	
	SelectRight();
	Super::RightShoulderPressed();
}

void AMenuPlayerController_B::FaceButtonBottomPressed()
{
	if (bIsInMenuMode)
		return;
	
	TrySelect();
}

void AMenuPlayerController_B::FaceButtonRightPressed()
{
	if (bIsInMenuMode)
		return;
	
	Unselect();
}

void AMenuPlayerController_B::TrySelect()
{
	if (PlayerCharacter)
		return;

	AMenuGameMode_B* MenuGameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (MenuGameMode)
		MenuGameMode->Select(this, CharacterVariantIndex);
}

void AMenuPlayerController_B::Unselect()
{

	AMenuGameMode_B* MenuGameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (MenuGameMode)
	{
		MenuGameMode->UnSelect(this);
	}
}

void AMenuPlayerController_B::SelectLeft()
{
	if (PlayerCharacter)
		return;

	AMenuGameMode_B* MenuGameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (MenuGameMode)
		MenuGameMode->HoverLeft(this);
}

void AMenuPlayerController_B::SelectRight()
{
	if (PlayerCharacter)
		return;

	AMenuGameMode_B* MenuGameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (MenuGameMode)
		MenuGameMode->HoverRight(this);
}

void AMenuPlayerController_B::SetIsMenuMode(const bool Value)
{
	bIsInMenuMode = Value;
}
