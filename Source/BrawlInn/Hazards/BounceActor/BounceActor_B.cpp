// Fill out your copyright notice in the Description page of Project Settings.

#include "BounceActor_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#include "BrawlInn.h"
#include "Hazards/BounceActor/BarrelTargetPoint_B.h"
#include "System/GameModes/GameMode_B.h"
#include "Characters/Player/PlayerCharacter_B.h"

ABounceActor_B::ABounceActor_B()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABounceActor_B::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(ExplodeTime);
	OnDestroyed.AddDynamic(this, &ABounceActor_B::Explode);
}

void ABounceActor_B::Explode(AActor* DestroyedActor)
{
	float Radius = 500.f;
	UGameplayStatics::ApplyRadialDamage(GetWorld(), 0, GetActorLocation(), Radius, BP_DamageType, {}, this);
	DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 10, FColor::Red, false, 2.f);
	if (Target)
		Target->SetActorHiddenInGame(true);

	//Respawns player. Kinda hates having this here.
	if (PlayerController)
	{
		AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->SpawnCharacter_D.Broadcast(PlayerController, true,FTransform(GetActorLocation()));
		}
	}
}
