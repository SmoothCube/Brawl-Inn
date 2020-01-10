// Fill out your copyright notice in the Description page of Project Settings.


#include "BouncePath_B.h"
#include "Components/SplineComponent.h"
#include "BounceActor_B.h"

#include "BounceActor_B.h"
// Sets default values
ABouncePath_B::ABouncePath_B()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SplineCurve = CreateDefaultSubobject<USplineComponent>("SplineCurve");
}

// Called when the game starts or when spawned
void ABouncePath_B::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABouncePath_B::Tick(float DeltaTime)
{
	TArray<ABounceActor_B*> ToRemove;
	Super::Tick(DeltaTime);
	for (auto& a : ActiveBounceActors)
	{
		a->SetActorLocation(SplineCurve->GetLocationAtTime(a->TimeStep, ESplineCoordinateSpace::World));
		a->TimeStep += DeltaTime * a->Speed;
		if (a->TimeStep >=1)
		{
			ToRemove.Add(a);
		}
	}
	for (auto& a : ToRemove)
	{
		ActiveBounceActors.Remove(a);
	}

}

void ABouncePath_B::AddBounceActor(ABounceActor_B* inBounceActor)
{
	ActiveBounceActors.Add(inBounceActor);
}
