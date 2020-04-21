// Fill out your copyright notice in the Description page of Project Settings.

#include "PauseMenu_B.h"

#include "ControllerLayout_B.h"
#include "Kismet/GameplayStatics.h"

#include "System/GameInstance_B.h"
#include "System/GameModes/GameMode_B.h"
#include "UI/UIElements/Button_B.h"

void UPauseMenu_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ContinueButton->OnClicked.AddDynamic(this, &UPauseMenu_B::ContinueButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &UPauseMenu_B::ExitButtonClicked);
	ControlsButton->OnClicked.AddDynamic(this, &UPauseMenu_B::ControlsButtonClicked);

	ControllerLayout->SetVisibility(ESlateVisibility::Collapsed);
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
	ControlsButton->Execute_Tick(ControlsButton);
	ExitButton->Execute_Tick(ExitButton);
}

void UPauseMenu_B::ContinueButtonClicked()
{
	AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	check(IsValid(GameMode));

	GameMode->ResumeGame();
}

void UPauseMenu_B::ControlsButtonClicked()
{
	ControllerLayout->SetVisibility(ESlateVisibility::Visible);

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(ControllerLayout->GetBackButton()->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);

	ControllerLayout->GetBackButton()->OnPressed.AddDynamic(this, &UPauseMenu_B::ControllerLayoutBackButtonClicked);
}

void UPauseMenu_B::ControllerLayoutBackButtonClicked()
{
	ControllerLayout->SetVisibility(ESlateVisibility::Collapsed);

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(ControlsButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);

	ControllerLayout->GetBackButton()->OnPressed.RemoveDynamic(this, &UPauseMenu_B::ControllerLayoutBackButtonClicked);
}

void UPauseMenu_B::ExitButtonClicked()
{
	UGameInstance_B* GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	check(IsValid(GameInstance));
	GameInstance->ReturnToMainMenu();
}
