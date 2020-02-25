// Fill out your copyright notice in the Description page of Project Settings.

#include "GameOverlay_B.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

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

	if (!GameInstance->GameIsScoreBased())
		TimeText->RemoveFromParent();

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
			if (HealthWidgets.IsValidIndex(Info.ID))
				HealthWidgets[Info.ID]->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UGameOverlay_B::UpdateTimerText(int TimeRemaining)
{
	if (TimeRemaining < 0 || !IsValid(TimeText))
		return;

	int Minutes = TimeRemaining / 60;
	int Seconds = TimeRemaining - 60 * Minutes;

	FNumberFormattingOptions Options;
	Options.MinimumIntegralDigits = 2;

	FText Display = FText::Join(FText::FromString(":"), FText::AsNumber(Minutes, &Options), FText::AsNumber(Seconds, &Options));
	TimeText->SetText(Display);
}
