// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticThrowable_B.h"
#include "Components/StaticMeshComponent.h"
#include "BrawlInn.h"

AStaticThrowable_B::AStaticThrowable_B()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
}

AThrowable_B* AStaticThrowable_B::PickedUp(APlayerCharacter_B* Owner)
{
	return Super::PickedUp(Owner);
}

void AStaticThrowable_B::Dropped()
{
	Super::Dropped();
	Mesh->SetSimulatePhysics(true);
}

UMeshComponent* AStaticThrowable_B::GetMesh()
{
	return Mesh;
}
