// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent_B.h"
#include "BrawlInn.h"
#include "UI/Game/HealthWidget_B.h"

#include "System/SubSystems/ScoreSubSystem_B.h"
#include "Characters/Player/PlayerController_B.h"


UHealthComponent_B::UHealthComponent_B()
{
	PrimaryComponentTick.bCanEverTick = false;
}

int UHealthComponent_B::GetHealth() const
{
	return Health;
}

int UHealthComponent_B::GetRespawns() const
{
	return Respawns;
}

void UHealthComponent_B::SetHealth(int Value)
{
	Health = Value;
	if (Health < 0)
		Health = 0;

	if (Health == 0)
	{
		Health = 100;
		Respawns--;
		if (Respawns <= 1)
			RespawnIsZero_D.Broadcast();
		else
			HealthIsZero_D.Broadcast();
	}

	HealthUpdated_D.Broadcast(Health);
}

void UHealthComponent_B::TakeDamage(int Value)
{
	SetHealth(Health - Value);
}


void UHealthComponent_B::BeginPlay()
{
	Super::BeginPlay();

	StartingHealth = Health;
}
