// Fill out your copyright notice in the Description page of Project Settings.

#include "VictoryScreenWidget_B.h"
#include "Components/TextBlock.h"
#include "UI/UIElements/Button_B.h"
#include "Kismet/GameplayStatics.h"
#include "System/DataTable_B.h"

void UVictoryScreenWidget_B::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerWon->SetText(FText::FromString("Player " + FString::FormatAsNumber(UGameplayStatics::GetPlayerControllerID(GetOwningPlayer()) + 1) + " won!"));

}

void UVictoryScreenWidget_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	ContinueButton->Execute_Tick(ContinueButton);
}

void UVictoryScreenWidget_B::ContinueButtonClicked()
{
	UDataTable_B* DataTable = NewObject<UDataTable_B>();
	DataTable->LoadCSVFile(FStringValues::StaticStruct(), "MapNames.csv");
	const FName LevelName = *DataTable->GetRow<FStringValues>("MenuMap")->Value;
	DataTable->ConditionalBeginDestroy();
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}

void UVictoryScreenWidget_B::EnableContinueButton()
{
	ContinueButton->OnClicked.AddDynamic(this, &UVictoryScreenWidget_B::ContinueButtonClicked);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ContinueButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
	ContinueButton->SetKeyboardFocus();
}
