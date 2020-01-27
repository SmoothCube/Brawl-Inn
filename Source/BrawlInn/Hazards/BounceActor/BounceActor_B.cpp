// Fill out your copyright notice in the Description page of Project Settings.

#include "BounceActor_B.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "BrawlInn.h"

#include "Player/PlayerCharacter_B.h"

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
	UGameplayStatics::ApplyRadialDamage(GetWorld(), 0, GetActorLocation(), 500.f, BP_DamageType, {}, this);
	Destroy();
}
