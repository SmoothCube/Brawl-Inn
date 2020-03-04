// Fill out your copyright notice in the Description page of Project Settings.

#include "Bar_B.h"
#include "Sound/SoundCue.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

#include "BrawlInn.h"
#include "Characters/AI/AICharacter_B.h"
#include "System/GameInstance_B.h"

ABar_B::ABar_B()
{
	PrimaryActorTick.bCanEverTick = false;

	House = CreateDefaultSubobject<UStaticMeshComponent>("House");
	SetRootComponent(House);

	Door = CreateDefaultSubobject<UStaticMeshComponent>("Door");
	Door->SetupAttachment(House);
}

void ABar_B::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Waiters;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), WaiterTag, Waiters);
	for (auto& Waiter : Waiters)
	{
		DropLocationMap.Add(Cast<AAICharacter_B>(Waiter), FDropLocations(EBarDropLocationType::Tankard));
		AvailableWaiters.Add(Cast<AAICharacter_B>(Waiter));
		GiveRandomTankard(Cast<AAICharacter_B>(Waiter));
	}

	TArray<AActor*> StoolReplacers;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), StoolReplacerTag, StoolReplacers);
	for (auto& StoolReplacer : StoolReplacers)
	{
		DropLocationMap.Add(Cast<AAICharacter_B>(StoolReplacer), FDropLocations(EBarDropLocationType::Stool));
		AvailableStoolReplacer.Add(Cast<AAICharacter_B>(StoolReplacer));
		Cast<AAICharacter_B>(StoolReplacer)->ItemDelivered = BP_Stool.GetDefaultObject();
	}
}

void ABar_B::GiveRandomTankard(AAICharacter_B* Waiter)
{
	if (BP_Useables.Num() == 0)
		return;

	const int RandomIndex = FMath::RandRange(0, BP_Useables.Num() - 1);
	if (BP_Useables.IsValidIndex(RandomIndex))
		Waiter->ItemDelivered = BP_Useables[RandomIndex].GetDefaultObject();
}

void ABar_B::AddDropLocation(EBarDropLocationType Type, AAIDropPoint_B* DropPoint)
{
	switch (Type)
	{
	case EBarDropLocationType::Stool:
		CurrentStoolReplacerIndex = (CurrentStoolReplacerIndex + 1) % AvailableStoolReplacer.Num();
		if (AvailableStoolReplacer.IsValidIndex(CurrentStoolReplacerIndex))
			DropLocationMap.Find(AvailableStoolReplacer[CurrentStoolReplacerIndex])->AddBack(DropPoint);
		break;
	case EBarDropLocationType::Tankard:
		CurrentWaiterIndex = (CurrentWaiterIndex + 1) % AvailableWaiters.Num();
		if (AvailableWaiters.IsValidIndex(CurrentWaiterIndex))
		{
			DropLocationMap.Find(AvailableWaiters[CurrentWaiterIndex])->AddBack(DropPoint);
			GiveRandomTankard(AvailableWaiters[CurrentWaiterIndex]);
		}
		break;
	default:;
	}
}

FDropLocations* ABar_B::GetDropLocations(AAICharacter_B* Character)
{
	return DropLocationMap.Find(Character);
}
