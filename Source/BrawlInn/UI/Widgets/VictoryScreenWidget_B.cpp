// Fill out your copyright notice in the Description page of Project Settings.

#include "VictoryScreenWidget_B.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "System/GameInstance_B.h"
#include "System/SubSystems/ScoreSubSystem_B.h"
#include "UI/UIElements/Button_B.h"

void UVictoryScreenWidget_B::NativeConstruct()
{
	Super::NativeConstruct();

	//UGameInstance_B* GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	/*TArray<UTextBlock*> PunchTextBlocks;
	PunchTextBlocks.Add(FirstPlacePunches);
	PunchTextBlocks.Add(SecondPlacePunches);
	PunchTextBlocks.Add(ThirdPlacePunches);
	PunchTextBlocks.Add(FourthPlacePunches);

	for(auto TextBlock : PunchTextBlocks)
		TextBlock->SetVisibility(ESlateVisibility::Hidden);

	for (int i = 0; i < GameInstance->GetPlayerInfos().Num(); i++)
	{
		const int PunchesHit = GameInstance->GetLocalPlayerByIndex(GameInstance->GetPlayerInfos()[i].ID)->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().PunchesHit;
		PunchTextBlocks[i]->SetText(FText::AsNumber(PunchesHit));
		PunchTextBlocks[i]->SetVisibility(ESlateVisibility::Visible);
	}*/
}

void UVictoryScreenWidget_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	ContinueButton->Execute_Tick(ContinueButton);
}

float UVictoryScreenWidget_B::easeInOut(float t)
{
	return t < .5f ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;
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
