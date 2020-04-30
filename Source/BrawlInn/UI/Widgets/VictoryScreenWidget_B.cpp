// Fill out your copyright notice in the Description page of Project Settings.

#include "VictoryScreenWidget_B.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "VerticalBox.h"
#include "VerticalBoxSlot.h"

#include "BrawlInn.h"
#include "Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "System/GameInstance_B.h"
#include "System/SubSystems/ScoreSubSystem_B.h"
#include "UI/UIElements/Button_B.h"
#include "Victory/CharacterVictoryScreen_B.h"
#include "Victory/StatEntry_B.h"

void UVictoryScreenWidget_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	StatBoards.Add(StatTopLeft);
	StatBoards.Add(StatTopRight);
	StatBoards.Add(StatBotLeft);
	StatBoards.Add(StatBotRight);

	for (auto Board : StatBoards)
	{
		Board->StatsBox->ClearChildren();
		for (int i = 0; i < 5; ++i)
		{
			UStatEntry_B* Item = CreateWidget<UStatEntry_B>(Board, BP_StatEntry);
			UVerticalBoxSlot* VSlot = Board->StatsBox->AddChildToVerticalBox(Item);
			VSlot->SetPadding(FMargin(0, 0, 0, 10.f));
		}
	}

		//Update backgroundcolors
	if (BackgroundColorsInOrder.Num() != 0 && BannerBotInOrder.Num() != 0 && BannerTopInOrder.Num() != 0)
	{
		const TArray<FPlayerInfo> PlayerInfos = GameInstance->GetPlayerInfos();
		for (int i = 0; i < PlayerInfos.Num(); ++i)
		{
			if (StatBoards[i]->BackgroundMaterial)
			{
				StatBoards[i]->BackgroundMaterial->SetVectorParameterValue("BaseColor", BackgroundColorsInOrder[static_cast<int>(PlayerInfos[i].Type)]);
				StatBoards[i]->BannerBot->Brush.SetResourceObject(BannerBotInOrder[static_cast<int>(PlayerInfos[i].Type)]);
				StatBoards[i]->BannerTop->Brush.SetResourceObject(BannerTopInOrder[static_cast<int>(PlayerInfos[i].Type)]);
				BLog("%i", static_cast<int>(PlayerInfos[i].Type));
			}
		}

	}


	CountNumbers.AddDefaulted(4);

	DisplayScores(PunchesHit);
	DisplayScores(OutOfMapDeaths);
	DisplayScores(CrownTime);
	DisplayScores(ThrowablesHit);
	DisplayScores(BarrelsHit);
	DisplayScores(Score);

}

void UVictoryScreenWidget_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	ContinueButton->Execute_Tick(ContinueButton);

	for (auto& Count : CountNumbers)
	{
		if (Count.Size() == 0)
			continue;


		FormattingOptions.UseGrouping = false;

		if (Count.Top().CurrentTime < Count.Top().Duration)
		{
			Count.Top().CurrentTime += InDeltaTime;
			const int ValueFloored = FMath::FloorToInt(UKismetMathLibrary::FInterpEaseInOut(
				Count.Top().Start, Count.Top().End, Count.Top().CurrentTime / Count.Top().Duration, 2));

			if (Count.Top().TextBlock)
				Count.Top().TextBlock->SetText(FText::AsNumber(ValueFloored, &FormattingOptions));
		}
		else
		{
			if (Count.Top().TextBlock)
				Count.Top().TextBlock->SetText(FText::AsNumber(Count.Top().End, &FormattingOptions));
			Count.Pop();
		}
	}
}

void UVictoryScreenWidget_B::ContinueButtonClicked()
{
	GameInstance = Cast<UGameInstance_B>(GetGameInstance());
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

		if (!StatBoards[i]->IsVisible())
			StatBoards[i]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);


		switch (Type) {
		case Score:
			AddToCountQueue(0, ScoreValues.Score, StatBoards[i]->ScoreNumber, PlayerInfos[i].ID);
			break;
		case PunchesHit:
		{
			UStatEntry_B* Stat = Cast<UStatEntry_B>(StatBoards[i]->StatsBox->GetChildAt(0));
			Stat->Text->SetText(FText::FromString("PUNCHES HIT"));
			AddToCountQueue(0, ScoreValues.PunchesHit, Stat->Number, PlayerInfos[i].ID);
			break;
		}
		case OutOfMapDeaths:
		{

			UStatEntry_B* Stat = Cast<UStatEntry_B>(StatBoards[i]->StatsBox->GetChildAt(1));
			Stat->Text->SetText(FText::FromString("OUT OF MAP"));
			AddToCountQueue(0, ScoreValues.OutOfMapDeaths, Stat->Number, PlayerInfos[i].ID);
			break;
		}
		case CrownTime:
		{
			UStatEntry_B* Stat = Cast<UStatEntry_B>(StatBoards[i]->StatsBox->GetChildAt(2));
			Stat->Text->SetText(FText::FromString("CROWN TIME (S)"));
			AddToCountQueue(0, ScoreValues.CrownTime, Stat->Number, PlayerInfos[i].ID);
			break;
		}
		case ThrowablesHit:
		{
			UStatEntry_B* Stat = Cast<UStatEntry_B>(StatBoards[i]->StatsBox->GetChildAt(3));
			Stat->Text->SetText(FText::FromString("THROWABLES HIT"));
			AddToCountQueue(0, ScoreValues.ThrowablesHit, Stat->Number, PlayerInfos[i].ID);
			break;
		}
		case BarrelsHit:
		{

			UStatEntry_B* Stat = Cast<UStatEntry_B>(StatBoards[i]->StatsBox->GetChildAt(4));
			Stat->Text->SetText(FText::FromString("Barrels Hit"));
			AddToCountQueue(0, ScoreValues.BarrelsHit, Stat->Number, PlayerInfos[i].ID);
			break;
		}
		default:;
		}
	}

}

void UVictoryScreenWidget_B::AddToCountQueue(int Start, int End, UTextBlock* TextBlock, int PlayerControllerID)
{
	if (!TextBlock)
		return;

	CountNumbers[PlayerControllerID].Push(FCountNumber(End, 1, TextBlock));

	//	TextBlock->SetText(FText::AsNumber(End));
}
