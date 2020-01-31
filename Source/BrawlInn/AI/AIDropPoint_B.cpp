// Fill out your copyright notice in the Description page of Project Settings.


#include "AIDropPoint_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Items/Item_B.h"
#include "BrawlInn.h"
#include "AI/AIItemManager_B.h"

void AAIDropPoint_B::BeginPlay()
{
	Super::BeginPlay();

	Item = GetWorld()->SpawnActor<AItem_B>(ItemToSpawn, GetActorTransform());
	if (Item)
	{
		Item->OnDestroyed.AddDynamic(this, &AAIDropPoint_B::ItemDestroyed);
	}
}

void AAIDropPoint_B::ItemDestroyed(AActor* DestroyedActor)
{
	AAIItemManager_B* Manager = Cast<AAIItemManager_B>(UGameplayStatics::GetActorOfClass(GetWorld(), AAIItemManager_B::StaticClass()));
	if (Manager)
	{
		BScreen("New item requested");
		Manager->ItemQueue.Enqueue(this);
	}
}