// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleThrowable_B.h"
#include "DestructibleComponent.h"
#include "BrawlInn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ADestructibleThrowable_B::ADestructibleThrowable_B()
{
	DestructibleMesh = CreateDefaultSubobject<UDestructibleComponent>("Destructible Mesh");
	RootComponent = DestructibleMesh;
}

AThrowable_B* ADestructibleThrowable_B::PickedUp(APlayerCharacter_B* Owner)
{
	Super::PickedUp(Owner);
	Destroy();

	AThrowable_B* NewItem =  GetWorld()->SpawnActor<AThrowable_B>(BP_Throwable, GetActorLocation(), FRotator::ZeroRotator);
	NewItem->OwningPlayer = OwningPlayer;

	return NewItem;
}

void ADestructibleThrowable_B::Dropped()
{
	Super::Dropped();
}

UMeshComponent* ADestructibleThrowable_B::GetMesh()
{
	return DestructibleMesh;
}
