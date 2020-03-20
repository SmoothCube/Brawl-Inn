// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableDropPoint_B.h"
#include "Engine/World.h"

#include "Items/Item_B.h"
#include "Hazards/Bar_B.h"

void AThrowableDropPoint_B::SpawnItem()
{
	Item = GetWorld()->SpawnActor<AItem_B>(ItemToSpawn, GetActorTransform());
	if (Item)
		Item->OnFracture().AddUObject(this, &AThrowableDropPoint_B::ItemDestroyed);
}

void AThrowableDropPoint_B::ItemDestroyed()
{
	if (Bar)
		Bar->AddDropLocation(Type, this);
}
