// Fill out your copyright notice in the Description page of Project Settings.


#include "BounceActor_B.h"
#include "Components/StaticMeshComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"
// Sets default values
ABounceActor_B::ABounceActor_B()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
}

// Called when the game starts or when spawned
void ABounceActor_B::BeginPlay()
{
	Super::BeginPlay();
	SetActorRotation(FRotator(0, 50, 0));
	Mesh->AddForce(FVector(-25000, 25000, 0));

}

// Called every frame
void ABounceActor_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetActorRotation(FRotator(GetActorRotation().Pitch, 50, GetActorRotation().Roll));
	AddActorLocalRotation(FRotator(0,0, RotateSpeed * DeltaTime	));

}

