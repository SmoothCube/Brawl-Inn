// Fill out your copyright notice in the Description page of Project Settings.


#include "AOEDamageAreaSubsystem_B.h"
#include "BrawlInn.h"
#include "Kismet/GameplayStatics.h"
#include "Hazards/AOEDamageArea_B.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UAOEDamageAreaSubsystem_B::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAOEDamageArea_B::StaticClass(), OutActors);
	for (const auto& Actor : OutActors)
		DamageAreas.Add(Cast<AAOEDamageArea_B>(Actor));

	BScreen("%i", DamageAreas.Num());

	for (AAOEDamageArea_B* Area : DamageAreas)
	{
		float ActiveTime = FMath::RandRange(Area->MinActiveTime, Area->MaxActiveTime);
		GetWorld()->GetTimerManager().SetTimer(Area->TH_Timer, Area, &AAOEDamageArea_B::ActivateFire, ActiveTime,false);
	}
}

void UAOEDamageAreaSubsystem_B::Deinitialize()
{
	Super::Deinitialize();
}
