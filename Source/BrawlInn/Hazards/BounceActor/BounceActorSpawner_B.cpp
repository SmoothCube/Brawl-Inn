// Fill out your copyright notice in the Description page of Project Settings.


#include "BounceActorSpawner_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Hazards/BounceActor/BounceActor_B.h"
#include "Hazards/BounceActor/BouncePath_B.h"

// Sets default values
ABounceActorSpawner_B::ABounceActorSpawner_B()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ABounceActorSpawner_B::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(TH_SpawnTimer, this, &ABounceActorSpawner_B::SpawnBounceActor, SpawnDelay, true,0);
}

// Called every frame
void ABounceActorSpawner_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABounceActorSpawner_B::SpawnBounceActor()
{
	//spawns to path
	if (Paths.Num() > 0)
	{
		ABounceActor_B* NewBounceActor = GetWorld()->SpawnActor<ABounceActor_B>(ActorToSpawn, GetActorLocation(), FRotator(90, 0, 0));
		if(Paths[NextPath])
			Paths[NextPath]->AddBounceActor(NewBounceActor);
		else
			UE_LOG(LogTemp, Error, TEXT("[ABounceActorSpawner_B::SpawnBounceActor] Empty Slot in Path Array!"));

		//cycles through the different paths instead of random spawning
		NextPath++;
		if (NextPath >= Paths.Num())
		{
			NextPath = 0;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ABounceActorSpawner_B::SpawnBounceActor] No paths for barrel!"));
	}
}
