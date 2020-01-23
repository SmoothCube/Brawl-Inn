// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu_B.h"
#include "BrawlInn.h"
#include "UI/Buttons/Button_B.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "System/MenuGameMode_B.h"

bool UMainMenu_B::Initialize()
{
	bool success = Super::Initialize();

	GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));

	PlayButton->OnClicked.AddDynamic(this, &UMainMenu_B::PlayButtonClicked);
	SettingsButton->OnClicked.AddDynamic(this, &UMainMenu_B::SettingsButtonClicked);
	CreditsButton->OnClicked.AddDynamic(this, &UMainMenu_B::CreditsButtonClicked);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu_B::QuitButtonClicked);

	return success;
}

void UMainMenu_B::NativeConstruct()
{
	Super::NativeConstruct();

	Buttons.Add(PlayButton);
	Buttons.Add(SettingsButton);
	Buttons.Add(CreditsButton);
	Buttons.Add(QuitButton);

	PlayButton->SetKeyboardFocus();
}

void UMainMenu_B::MenuTick()
{
	for (const auto& Button : Buttons)
		Button->ButtonTick();
}

void UMainMenu_B::PlayButtonClicked()
{
	BScreen("Playbutton clicked");
	if (GameMode)
	{
		GameMode->LS_PlayGame();
		GameMode->HideMainMenu();
	}
}

void UMainMenu_B::SettingsButtonClicked()
{
	BScreen("Settingsbutton clicked");
}

void UMainMenu_B::CreditsButtonClicked()
{
	BScreen("Creditsbutton clicked");
}

void UMainMenu_B::QuitButtonClicked()
{
	BScreen("Quitbutton clicked");
	if (GameMode)
		GameMode->LS_QuitGame();
}
