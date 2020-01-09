// Fill out your copyright notice in the Description page of Project Settings.


#include "Throwable_B.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AThrowable_B::AThrowable_B()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;

	PickupSphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	PickupSphere->SetupAttachment(Mesh);
}

void AThrowable_B::PickedUp()
{
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bIsHeld = true;
}

void AThrowable_B::Dropped()
{
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
	bIsHeld = false;
}

bool AThrowable_B::IsHeld() const
{
	return bIsHeld;
}

// Called when the game starts or when spawned
void AThrowable_B::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AThrowable_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

