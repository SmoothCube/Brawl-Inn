// Fill out your copyright notice in the Description page of Project Settings.

#include "AIDropPoint_B.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "BrawlInn.h"
#include "Hazards/Bar_B.h"
#include "Items/Item_B.h"
#include "System/GameModes/MainGameMode_B.h"

void AAIDropPoint_B::BeginPlay()
{
	Super::BeginPlay();

	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	check(IsValid(GameMode));
	Bar = GameMode->GetBar();

	SpawnItem();
}

FOnItemDelivered& AAIDropPoint_B::OnItemDelivered()
{
	return OnItemDelivered_Delegate;
}

void AAIDropPoint_B::SetNewItem(AItem_B* NewItem)
{
	Item = NewItem;
	if (Item)
		Item->OnFracture().AddUObject(this, &AAIDropPoint_B::ItemDestroyed);

	if (GetSquaredDistanceTo(Item) < 10)
		OnItemDelivered_Delegate.Broadcast();
}
