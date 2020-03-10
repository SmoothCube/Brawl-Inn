// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenu_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "UI/Buttons/Button_B.h"
#include "System/GameModes/MainGameMode_B.h"
#include "BrawlInn.h"


bool UPauseMenu_B::Initialize()
{
	bool success = Super::Initialize();
	return success;
}

void UPauseMenu_B::NativeConstruct()
{
	Super::NativeConstruct();

	ContinueButton->OnClicked.AddDynamic(this, &UPauseMenu_B::ContinueButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &UPauseMenu_B::ExitButtonClicked);

	Buttons.Add(ContinueButton);
	Buttons.Add(ExitButton);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ContinueButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
	ContinueButton->SetKeyboardFocus();

}

void UPauseMenu_B::MenuTick()
{
	
	for (const auto& Button : Buttons)
		Button->ButtonTick();
}

void UPauseMenu_B::ContinueButtonClicked()
{
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameMode)
		return;

	GameMode->ResumeGame();
}

void UPauseMenu_B::ExitButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MenuMap_v2");
}
