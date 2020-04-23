// Fill out your copyright notice in the Description page of Project Settings.


#include "Bar_B.h"


#include "BrawlInn.h"
#include "Characters/AI/AICharacter_B.h"
#include "Characters/AI/IdleAICharacter_B.h"
#include "Kismet/GameplayStatics.h"
#include "System/DataTable_B.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/MainGameMode_B.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "Items/Useable_B.h"

UBar_B::UBar_B()
{

	PrimaryComponentTick.bCanEverTick = false;
}

void UBar_B::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (!GameInstance) { BError("%s can't find the GameInstance_B! ABORT", *GetNameSafe(this)); return; }

	if (GameInstance->ShouldUseSpreadSheets())
	{
		UDataTable_B* DataTable = NewObject<UDataTable_B>();
		DataTable->LoadCSVFile(FBarValues::StaticStruct(), "BarSettings.csv");
		TimeUntilFirstDelivery = DataTable->GetRow<FBarValues>("FirstDeliveryTime")->Value;
		TimeUntilDelivery = DataTable->GetRow<FBarValues>("DeliveryTime")->Value;
		DataTable->ConditionalBeginDestroy();
	}

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), WaiterTag, OutActors);
	for (auto WaiterActor : OutActors)
	{
		AAICharacter_B* Waiter = Cast<AAICharacter_B>(WaiterActor);
		DropLocationMap.Add(Waiter, FBarDropLocations(EBarDropLocationType::Tankard));
		Waiters.Add(Waiter);
		GiveRandomTankard(Waiter);
	}

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), StoolReplacerTag, Actors);
	for (auto StoolReplacerActor : Actors)
	{
		AAICharacter_B* StoolReplacer = Cast<AAICharacter_B>(StoolReplacerActor);
		DropLocationMap.Add(StoolReplacer, FBarDropLocations(EBarDropLocationType::Stool));
		StoolReplacers.Add(StoolReplacer);
		StoolReplacer->SetItemDelivered(BP_Stool.GetDefaultObject());
	}

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), BoxReplacerTag, Actors);
	for (auto BoxReplacerActor : Actors)
	{
		AAICharacter_B* BoxReplacer = Cast<AAICharacter_B>(BoxReplacerActor);
		DropLocationMap.Add(BoxReplacer, FBarDropLocations(EBarDropLocationType::Stool));
		BoxReplacers.Add(BoxReplacer);
		BoxReplacer->SetItemDelivered(BP_Box.GetDefaultObject());
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIdleAICharacter_B::StaticClass(), Actors);
	for (auto CustomerActor : Actors)
	{
		AIdleAICharacter_B* Customer = Cast<AIdleAICharacter_B>(CustomerActor);
		if (!Customer || !Customer->CanOrderDrink())
			continue;;
		Customers.Add(Customer);
	}

	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(GetOwner());
	if (GameMode)
	{
		GameMode->OnGameStart.AddLambda([&]()
			{
				StartRandomOrder(TimeUntilFirstDelivery);
			});
	}
}

void UBar_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	Super::EndPlay(EndPlayReason);
}

void UBar_B::GiveRandomTankard(AAICharacter_B* Waiter)
{
	if (BP_Useables.Num() == 0)
		return;

	const int RandomIndex = FMath::RandRange(0, BP_Useables.Num() - 1);
	if (BP_Useables.IsValidIndex(RandomIndex))
		Waiter->SetItemDelivered(BP_Useables[RandomIndex].GetDefaultObject());
}

void UBar_B::RandomOrder()
{
	const int RandomIndex = FMath::RandRange(0, Customers.Num() - 1);
	if (Customers.IsValidIndex(RandomIndex))
		Customers[RandomIndex]->OrderDrink();

	if (IsValid(DrinkReadySound))
	{
		UGameplayStatics::PlaySound2D(
			GetWorld(),
			DrinkReadySound,
			1.f
		);
	}
}

void UBar_B::StartRandomOrder(const float Time)
{
	GetWorld()->GetTimerManager().SetTimer(TH_StartOrderTimer, this, &UBar_B::RandomOrder, Time < 0 ? TimeUntilDelivery : Time, false);
}

void UBar_B::GetOrder(AAIDropPoint_B* DropPoint)
{
	AddDropLocation(EBarDropLocationType::Tankard, DropPoint);
}

void UBar_B::AddDropLocation(const EBarDropLocationType Type, AAIDropPoint_B* DropPoint)
{
	switch (Type)
	{
	case EBarDropLocationType::Stool:
		CurrentStoolReplacerIndex = (CurrentStoolReplacerIndex + 1) % StoolReplacers.Num();
		if (StoolReplacers.IsValidIndex(CurrentStoolReplacerIndex))
			DropLocationMap.Find(StoolReplacers[CurrentStoolReplacerIndex])->AddBack(DropPoint);
		break;
	case EBarDropLocationType::Tankard:
		CurrentWaiterIndex = (CurrentWaiterIndex + 1) % Waiters.Num();
		if (Waiters.IsValidIndex(CurrentWaiterIndex))
		{
			DropLocationMap.Find(Waiters[CurrentWaiterIndex])->AddBack(DropPoint);
			GiveRandomTankard(Waiters[CurrentWaiterIndex]);
		}
		break;
	case EBarDropLocationType::Box:
		CurrentBoxReplacerIndex = (CurrentBoxReplacerIndex + 1) % BoxReplacers.Num();
		if (BoxReplacers.IsValidIndex(CurrentBoxReplacerIndex))
			DropLocationMap.Find(BoxReplacers[CurrentBoxReplacerIndex])->AddBack(DropPoint);
		break;
	default:;
	}
}

FBarDropLocations* UBar_B::GetDropLocations(AAICharacter_B* Character)
{
	return DropLocationMap.Find(Character);
}

FOnDeliverStart& UBar_B::OnDeliverStart()
{
	return OnDeliverStart_Delegate;
}

