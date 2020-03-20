// Fill out your copyright notice in the Description page of Project Settings.

#include "AIDropPoint_B.h"

#include "BrawlInn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "Items/Item_B.h"
#include "Hazards/Bar_B.h"

void AAIDropPoint_B::BeginPlay()
{
	Bar = Cast<ABar_B>(UGameplayStatics::GetActorOfClass(GetWorld(), ABar_B::StaticClass()));

	Super::BeginPlay();

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
