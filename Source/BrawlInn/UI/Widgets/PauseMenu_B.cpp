// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenu_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "BrawlInn.h"
#include "System/DataTable_B.h"
#include "System/GameModes/MainGameMode_B.h"
#include "UI/UIElements/Button_B.h"

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
		Button->Execute_Tick(Button);
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
	UDataTable_B* DataTable = NewObject<UDataTable_B>();
	DataTable->LoadCSVFile(FStringValues::StaticStruct(), "MapNames.csv");
	const FName LevelName = *DataTable->GetRow<FStringValues>("MenuMap")->Value;
	DataTable->ConditionalBeginDestroy();
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}
