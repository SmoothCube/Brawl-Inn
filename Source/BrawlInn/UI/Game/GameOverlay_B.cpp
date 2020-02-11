// Fill out your copyright notice in the Description page of Project Settings.

#include "GameOverlay_B.h"
#include "Kismet/GameplayStatics.h"

#include "BrawlInn.h"
#include "System/GameModes/MainGameMode_B.h"
#include "System/GameInstance_B.h"
#include "UI/Game/HealthWidget_B.h"
#include "Characters/Player/PlayerController_B.h"
#include "Components/HealthComponent_B.h"

void UGameOverlay_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	HealthWidgets.Add(Player0);
	HealthWidgets.Add(Player1);
	HealthWidgets.Add(Player2);
	HealthWidgets.Add(Player3);

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	if (GameInstance)
		GameInstance->OnPlayerInfoChanged.AddUObject(this, &UGameOverlay_B::ChangeHealthWidgetVisibility);

	/// Setup Icons and connect the health widgets to playercontrollers
	TArray<ULocalPlayer*> Players = GameInstance->GetLocalPlayers();
	for (ULocalPlayer* PlayerID : Players)
	{
		HealthWidgets[PlayerID->GetControllerId()]->SetVisibility(ESlateVisibility::Visible);
		APlayerController_B* PlayerController = Cast<APlayerController_B>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), PlayerID->GetControllerId()));
		if (PlayerController)
		{
			HealthWidgets[PlayerID->GetControllerId()]->SetOwningPlayer(PlayerController);
			PlayerController->HealthComponent->SetHealthWidget(HealthWidgets[PlayerID->GetControllerId()]);
		}
	}

	ChangeHealthWidgetVisibility();
}

void UGameOverlay_B::ChangeHealthWidgetVisibility()
{
	if (GameInstance)
	{
		for (auto& HealthWidget : HealthWidgets)
			HealthWidget->SetVisibility(ESlateVisibility::Hidden);

		TArray<FPlayerInfo> Players = GameInstance->GetPlayerInfos();
		for (FPlayerInfo Info : Players)
		{
			if(HealthWidgets.IsValidIndex(Info.ID))
				HealthWidgets[Info.ID]->SetVisibility(ESlateVisibility::Visible);
		}
	}
}