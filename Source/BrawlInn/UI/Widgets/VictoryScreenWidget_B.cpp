// Fill out your copyright notice in the Description page of Project Settings.

#include "VictoryScreenWidget_B.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "ListView.h"
#include "TimerManager.h"

#include "System/GameInstance_B.h"
#include "System/SubSystems/ScoreSubSystem_B.h"
#include "UI/UIElements/Button_B.h"
#include "Victory/CharacterVictoryScreen_B.h"
#include "Victory/StatEntry_B.h"

void UVictoryScreenWidget_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	EnableContinueButton(); // Enable etterhvert? Ikke instant

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	StatBoards.Add(StatTopLeft);
	StatBoards.Add(StatTopRight);
	StatBoards.Add(StatBotLeft);
	StatBoards.Add(StatBotRight);

	for (auto Board : StatBoards)
	{
		for (int i = 0; i < 3; ++i)
		{
			UStatEntry_B* Item = CreateWidget<UStatEntry_B>(Board, BP_StatEntry);
			Board->StatsList->AddItem(Item);
		}
	}

	DisplayScores(PunchesHit);
	
}

void UVictoryScreenWidget_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	ContinueButton->Execute_Tick(ContinueButton);
}

void UVictoryScreenWidget_B::ContinueButtonClicked()
{
	UGameInstance_B* GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	check(IsValid(GameInstance));
	GameInstance->ReturnToMainMenu();
}

void UVictoryScreenWidget_B::EnableContinueButton()
{
	ContinueButton->OnClicked.AddDynamic(this, &UVictoryScreenWidget_B::ContinueButtonClicked);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ContinueButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);
	ContinueButton->SetKeyboardFocus();
}

void UVictoryScreenWidget_B::DisplayScores(const EScoreValueTypes Type)
{
	const TArray<FPlayerInfo> PlayerInfos = GameInstance->GetPlayerInfos();
	for (int i = 0; i < PlayerInfos.Num(); ++i)
	{
		const FScoreValues ScoreValues = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), PlayerInfos[i].ID)
			->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues();

		switch (Type) {
		case Score:
			CountNumber(0, ScoreValues.Score, StatBoards[i]->ScoreNumber);
			break;
		case PunchesHit:
		{
			UStatEntry_B* Stat = Cast<UStatEntry_B>(StatBoards[i]->StatsList->GetItemAt(0));
			Stat->Text->SetText(FText::FromString("PUNCHES HIT"));
			CountNumber(0, ScoreValues.PunchesHit, Stat->Number);
			break;
		}
		case OutOfMapDeaths:
		{

			UStatEntry_B* Stat = Cast<UStatEntry_B>(StatBoards[i]->StatsList->GetItemAt(1));
			Stat->Text->SetText(FText::FromString("OUT OF MAP"));
			CountNumber(0, ScoreValues.OutOfMapDeaths, Stat->Number);
			break;
		}
		case CrownTime:
		{
			UStatEntry_B* Stat = Cast<UStatEntry_B>(StatBoards[i]->StatsList->GetItemAt(2));
			Stat->Text->SetText(FText::FromString("CROWN TIME (S)"));
			CountNumber(0, ScoreValues.CrownTime, Stat->Number);
			break;
		}
		case ThrowablesHit:
		{
			UStatEntry_B* Stat = Cast<UStatEntry_B>(StatBoards[i]->StatsList->GetItemAt(3));
			Stat->Text->SetText(FText::FromString("THROWABLES HIT"));
			CountNumber(0, ScoreValues.ThrowablesHit, Stat->Number);
			break;
		}
		default:;
		}
	}

}

//Count kommer etterhvert
void UVictoryScreenWidget_B::CountNumber(int Start, int End, UTextBlock* TextBlock)
{
	if (!TextBlock)
		return;

	TextBlock->SetText(FText::AsNumber(End));
}
