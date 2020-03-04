// Fill out your copyright notice in the Description page of Project Settings.

#include "Bar_B.h"
#include "Sound/SoundCue.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

#include "BrawlInn.h"
#include "Characters/AI/AICharacter_B.h"
#include "Components/BarMeshComponent_B.h"
#include "System/GameInstance_B.h"

ABar_B::ABar_B()
{
	PrimaryActorTick.bCanEverTick = false;

	House = CreateDefaultSubobject<UBarMeshComponent_B>("House");
	SetRootComponent(House);

	Door = CreateDefaultSubobject<UStaticMeshComponent>("Door");
	Door->SetupAttachment(House);

	ItemSpawnLocation = CreateDefaultSubobject<USceneComponent>("Item spawnlocation");
	ItemSpawnLocation->SetupAttachment(House);
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

	}


	TArray<AActor*> StoolReplacers;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), StoolReplacerTag, StoolReplacers);
	for (auto& StoolReplacer : StoolReplacers)
	{
		DropLocationMap.Add(Cast<AAICharacter_B>(StoolReplacer), FDropLocations(EBarDropLocationType::Stool));
		AvailableStoolReplacer.Add(Cast<AAICharacter_B>(StoolReplacer));
		Cast<AAICharacter_B>(StoolReplacer)->ItemDelivered = BP_Stool.GetDefaultObject();
	}
	StartTimerForNextTankard();
	//StartTimerForNextStool();

}

USceneComponent* ABar_B::GetItemSpawnLocation() const
{
	return ItemSpawnLocation;
}

void ABar_B::StartTimerForNextTankard()
{
	GetWorld()->GetTimerManager().SetTimer(TH_NextTankardTimer, this, &ABar_B::SpawnTankard, FMath::FRandRange(MinTankardSpawnTimer, MaxTankardSpawnTimer), false);
}

void ABar_B::SpawnTankard()
{
	if (BP_Useables.Num() == 0)
		return;

	const int RandomIndex = FMath::RandRange(0, BP_Useables.Num() - 1);
	AUseable_B* Item = GetWorld()->SpawnActor<AUseable_B>(BP_Useables[RandomIndex], House->GetSocketTransform(ItemSocket));
	Item->Tags.Add("AITankard");
	Item->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform, ItemSocket);
	for(auto& Waiter: AvailableWaiters)
	{
		Waiter->ItemDelivered = BP_Useables[RandomIndex].GetDefaultObject();
		
	}


}
void ABar_B::StartTimerForNextStool()
{
	GetWorld()->GetTimerManager().SetTimer(TH_NextStoolTimer, this, &ABar_B::SpawnStool, FMath::FRandRange(MinStoolSpawnTimer, MaxStoolSpawnTimer), false);
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
			DropLocationMap.Find(AvailableWaiters[CurrentWaiterIndex])->AddBack(DropPoint);
		break;
	default:;
	}
}

void ABar_B::SpawnStool()
{
	////AItem_B* StoolToDeliver = GetWorld()->SpawnActor<AItem_B>(BP_Stool, ItemSpawnLocation->GetComponentTransform());
	////StoolToDeliver->Tags.Add("AIStool");

	//CurrentStoolReplacerIndex = (CurrentStoolReplacerIndex + 1) % AvailableStoolReplacer.Num();
	//BLog("Next index: %i", CurrentStoolReplacerIndex);
	///if (AvailableStoolReplacer.IsValidIndex(CurrentStoolReplacerIndex))
	//	OnDeliverStart.Broadcast(BP_Stool.GetDefaultObject(), AvailableStoolReplacer[CurrentStoolReplacerIndex]);
}

FDropLocations* ABar_B::GetDropLocations(AAICharacter_B* Character)
{
	return DropLocationMap.Find(Character);
}
