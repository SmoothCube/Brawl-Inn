// Fill out your copyright notice in the Description page of Project Settings.

#include "AICharacter_B.h"
#include "System/GameModes/MainGameMode_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "Hazards/Bar_B.h"
#include "Items/Item_B.h"

void AAICharacter_B::BeginPlay()
{
	Super::BeginPlay();

	StartTransform = GetActorTransform();
	ABar_B* Bar = Cast<ABar_B>(UGameplayStatics::GetActorOfClass(GetWorld(), ABar_B::StaticClass()));
	if (Bar)
		Bar->OnDeliverStart().AddUObject(this, &AAICharacter_B::Deliver);
}

void AAICharacter_B::FellOutOfWorld(const UDamageType& DmgType)
{
	if (bShouldRespawn)
		SetActorTransform(StartTransform);
	else
		Super::FellOutOfWorld(DmgType);
}

void AAICharacter_B::Deliver(AItem_B* ItemToDeliver, AAICharacter_B* CharacterToDeliver)
{
	if (CharacterToDeliver == this)
		ItemDelivered = ItemToDeliver;
}

FTransform AAICharacter_B::GetStartTransform() const
{
	return StartTransform;
}

AItem_B* AAICharacter_B::GetItemDelivered() const
{
	return ItemDelivered;
}

void AAICharacter_B::SetItemDelivered(AItem_B* Item)
{
	ItemDelivered = Item;
}
