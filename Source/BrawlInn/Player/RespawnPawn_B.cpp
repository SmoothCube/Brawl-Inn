// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnPawn_B.h"
#include "Components/DecalComponent.h" 
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "BrawlInn.h"

#include "Player/PlayerController_B.h"
#include "System/GameMode_B.h"
#include "Hazards/BounceActor/BounceActorSpawner_B.h"
#include "Hazards/BounceActor/BounceActor_B.h"

// Sets default values
ARespawnPawn_B::ARespawnPawn_B()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Decal = CreateDefaultSubobject<UDecalComponent>("Decal");
	Decal->AddLocalRotation(FRotator(90, 0, 0));
}

// Called when the game starts or when spawned
void ARespawnPawn_B::BeginPlay()
{
	Super::BeginPlay();
	BWarn("RespawnPawn spawned");
	GetWorld()->GetTimerManager().SetTimer(TH_ThrowTimer, this, &ARespawnPawn_B::ThrowBarrel, TimeUntilThrow);
}

// Called every frame
void ARespawnPawn_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldOffset(InputVector * DeltaTime * MovementSpeed);
}

void ARespawnPawn_B::ThrowBarrel()
{
	if (!bBarrelIsThrown)
	{
		BWarn("Throwing Barrel!");
		ABounceActorSpawner_B* BarrelSpawner = Cast<ABounceActorSpawner_B>(UGameplayStatics::GetActorOfClass(GetWorld(), ABounceActorSpawner_B::StaticClass()));

		Barrel = BarrelSpawner->SpawnBounceActor(GetActorLocation());
		GetWorld()->GetTimerManager().ClearTimer(TH_ThrowTimer);
	
		bBarrelIsThrown = true;
	}
	else
	{
		if (Barrel)
		{
			Barrel->Destroy();
		}

		AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			APlayerController_B* PlayerController = Cast<APlayerController_B>(GetController());
			GameMode->SpawnCharacter_D.Broadcast(PlayerController);
		}
		else
		{
			BError("GameMode Could Not Be Found!");
		}
	}

}

