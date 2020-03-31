// Fill out your copyright notice in the Description page of Project Settings.

#include "PauseMenu_B.h"
#include "Kismet/GameplayStatics.h"

#include "System/GameInstance_B.h"
#include "System/GameModes/MainGameMode_B.h"
#include "UI/UIElements/Button_B.h"

void UPauseMenu_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ContinueButton->OnClicked.AddDynamic(this, &UPauseMenu_B::ContinueButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &UPauseMenu_B::ExitButtonClicked);
}

void UPauseMenu_B::NativeConstruct()
{
	Super::NativeConstruct();

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(ContinueButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
	ContinueButton->SetKeyboardFocus();
}

void UPauseMenu_B::RemoveFromParent()
{
	Super::RemoveFromParent();

	if (GetOwningPlayer())
	{
		const FInputModeGameOnly InputMode;
		GetOwningPlayer()->SetInputMode(InputMode);
	}
}

void UPauseMenu_B::MenuTick()
{
	ContinueButton->Execute_Tick(ContinueButton);
	ExitButton->Execute_Tick(ExitButton);
}

void UPauseMenu_B::ContinueButtonClicked()
{
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	check(IsValid(GameMode));

	GameMode->ResumeGame();
}

void UPauseMenu_B::ExitButtonClicked()
{
	UGameInstance_B* GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	check(IsValid(GameInstance));
	GameInstance->ReturnToMainMenu();
}
