// Fill out your copyright notice in the Description page of Project Settings.


#include "VictoryScreenWidget_B.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Player/PlayerController_B.h"


bool UVictoryScreenWidget_B::Initialize()
{
	return Super::Initialize();
}

void UVictoryScreenWidget_B::NativeConstruct()
{
	Super::NativeConstruct();

	VictoryImage->SetVisibility(ESlateVisibility::Hidden);
	PlayerWon->SetVisibility(ESlateVisibility::Hidden);
}

void UVictoryScreenWidget_B::OnPlayerWin(APlayerController_B* PlayerThatWon)
{
	VictoryImage->SetVisibility(ESlateVisibility::Visible);
	PlayerWon->SetVisibility(ESlateVisibility::Visible);
	PlayerWon->SetText(FText::FromString("Player " + FString::FormatAsNumber(UGameplayStatics::GetPlayerControllerID(PlayerThatWon) + 1) + " won!"));
}
