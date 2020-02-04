// Fill out your copyright notice in the Description page of Project Settings.

#include "GameOverlay_B.h"
#include "HealthWidget_B.h"
#include "System/GameInstance_B.h"
#include "Kismet/GameplayStatics.h"
#include "System/MainGameMode_B.h"

bool UGameOverlay_B::Initialize()
{
	bool s = Super::Initialize();

	HealthWidgets.Add(Player0);
	HealthWidgets.Add(Player1);
	HealthWidgets.Add(Player2);
	HealthWidgets.Add(Player3);

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	if (GameInstance)
		GameInstance->OnNumberPlayerControllersChanged.AddUObject(this, &UGameOverlay_B::UpdateHealthWidgets);

	UpdateHealthWidgets();

	return s;
}

void UGameOverlay_B::UpdateHealthWidgets()
{
	if (GameInstance)
	{
		for (auto& HealthWidget : HealthWidgets)
		{
			HealthWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		TArray<int> Players = GameInstance->GetActivePlayerControllerIDs();
		for (int PlayerID : Players)
			HealthWidgets[PlayerID]->SetVisibility(ESlateVisibility::Visible);
	}
}