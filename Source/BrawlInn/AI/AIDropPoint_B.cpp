// Fill out your copyright notice in the Description page of Project Settings.

#include "AIDropPoint_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "Items/Item_B.h"

void AAIDropPoint_B::BeginPlay()
{
	Bar = Cast<ABar_B>(UGameplayStatics::GetActorOfClass(GetWorld(), ABar_B::StaticClass()));

	Super::BeginPlay();

	Item = GetWorld()->SpawnActor<AItem_B>(ItemToSpawn, GetActorTransform());
	if (Item)
	{
		Item->OnFracture().AddUObject(this, &AAIDropPoint_B::ItemDestroyed);
	}
}

void AAIDropPoint_B::ItemDestroyed()
{
	if (Bar)
	{
		Bar->AddDropLocation(Type,this);
	}
}

void AAIDropPoint_B::SetNewItem(AItem_B* NewItem)
{
	Item = NewItem;
	if (Item)
	{
		Item->OnFracture().AddUObject(this, &AAIDropPoint_B::ItemDestroyed);
	}
}
