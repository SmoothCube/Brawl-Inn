// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu_B.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "BrawlInn.h"
#include "UI/Buttons/Button_B.h"
#include "System/GameModes/MenuGameMode_B.h"

bool UMainMenu_B::Initialize()
{
	bool success = Super::Initialize();
	return success;
}

void UMainMenu_B::NativeConstruct()
{
	Super::NativeConstruct();

	GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));

	PlayButton->OnClicked.AddDynamic(this, &UMainMenu_B::PlayButtonClicked);
	SettingsButton->OnClicked.AddDynamic(this, &UMainMenu_B::SettingsButtonClicked);
	CreditsButton->OnClicked.AddDynamic(this, &UMainMenu_B::CreditsButtonClicked);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu_B::QuitButtonClicked);

	Buttons.Add(PlayButton);
	Buttons.Add(SettingsButton);
	Buttons.Add(CreditsButton);
	Buttons.Add(QuitButton);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(PlayButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
}

void UMainMenu_B::MenuTick()
{
	for (const auto& Button : Buttons)
		Button->ButtonTick();
}

void UMainMenu_B::PlayButtonClicked()
{
	if (GameMode)
	{
		GameMode->LS_PlayGame();
		GameMode->HideMainMenu();
	}
}

void UMainMenu_B::SettingsButtonClicked()
{
	BScreen("Settings is disabled");
}

void UMainMenu_B::CreditsButtonClicked()
{
	BScreen("Credits: SmoothCube");
}

void UMainMenu_B::QuitButtonClicked()
{
	if (GameMode)
		GameMode->LS_QuitGame();
}
