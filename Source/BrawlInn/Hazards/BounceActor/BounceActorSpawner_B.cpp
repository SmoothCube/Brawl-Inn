// Fill out your copyright notice in the Description page of Project Settings.


#include "BounceActorSpawner_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "BrawlInn.h"

#include "Hazards/BounceActor/BarrelTargetPoint_B.h"
#include "Hazards/BounceActor/BounceActor_B.h"
#include "System/GameInstance_B.h"


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
	GetWorld()->GetTimerManager().SetTimer(TH_SpawnTimer, this, &ABounceActorSpawner_B::SpawnBarrelOnTimer, SpawnDelay, true, 0);
}

// Called every frame
void ABounceActorSpawner_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABounceActorSpawner_B::SpawnBarrelOnTimer()
{
	//cycles through the different paths instead of random spawning
	if (BouncePoints.IsValidIndex(NextPath) && BouncePoints[NextPath])
	{
		BouncePoints[NextPath]->SetActorHiddenInGame(false);
		ABounceActor_B* NewBounceActor = SpawnBounceActor(BouncePoints[NextPath]->GetActorLocation());
		if (IsValid(NewBounceActor))
			NewBounceActor->Target = BouncePoints[NextPath];

		if (SpawnCue)
		{
			float volume = 1.f;
			UGameInstance_B* GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
			if (GameInstance)
			{
				volume *= GameInstance->MasterVolume * GameInstance->SfxVolume;
			}
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				SpawnCue,
				GetActorLocation(),
				volume
			);
		}
	}
	else
	{
		ABounceActor_B* NewBounceActor = SpawnBounceActor(FVector::ZeroVector);
	}

	NextPath++;
	if (NextPath >= BouncePoints.Num())
	{
		NextPath = 0;
	}
}

ABounceActor_B* ABounceActorSpawner_B::SpawnBounceActor(FVector TargetLocation)
{
	ABounceActor_B* NewBounceActor = GetWorld()->SpawnActor<ABounceActor_B>(ActorToSpawn, GetActorLocation(), FRotator(90, 0, 0));
	if (!IsValid(NewBounceActor))
		return nullptr;

	NewBounceActor->SetActorRotation(FRotator(0, 50, 0));
	FVector LaunchVel = FVector::ZeroVector;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), LaunchVel, NewBounceActor->GetActorLocation(), TargetLocation, 0.0f, 0.5f);
	NewBounceActor->Mesh->AddImpulse(LaunchVel, NAME_None, true);

	return NewBounceActor;
}
