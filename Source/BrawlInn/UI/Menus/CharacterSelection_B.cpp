// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSelection_B.h"
#include "Components/TextBlock.h"
#include "System/MenuGameMode_B.h"
#include "Player/PlayerController_B.h"
#include "Kismet/GameplayStatics.h"

bool UCharacterSelection_B::Initialize()
{
	bool success = Super::Initialize();

	GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->SpawnCharacter_NOPARAM_D.AddDynamic(this, &UCharacterSelection_B::UpdateNumberOfPlayersText);
		GameMode->DespawnCharacter_NOPARAM_D.AddDynamic(this, &UCharacterSelection_B::UpdateNumberOfPlayersText);
	}

	return success;
}

void UCharacterSelection_B::UpdateNumberOfPlayersText()
{
	if (!IsValid(GameMode))
		return;

	int NumberOfPlayers = 0;

	for (const auto& Controller : GameMode->PlayerControllers)
	{
		if (Cast<APlayerController_B>(Controller)->HasValidCharacter())
			NumberOfPlayers++;
	}

	NumberOfPlayersText->SetText(FText::FromString(FString("Number of players: ") + FString::FormatAsNumber(NumberOfPlayers)));

}