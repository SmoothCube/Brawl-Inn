// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent_B.h"
#include "BrawlInn.h"


UHealthComponent_B::UHealthComponent_B()
{
	PrimaryComponentTick.bCanEverTick = false;
}

int UHealthComponent_B::GetHealth() const
{
	return Health;
}

void UHealthComponent_B::SetHealth(int Value)
{
	Health = Value;
	if (Health < 0)
		Health = 0;

	BScreen("Health:  %i", Health);
	HealthUpdated_D.Broadcast(Health);

	if (Health == 0)
		HealthIsZero_D.Broadcast();
}

void UHealthComponent_B::TakeDamage(int Value)
{
	SetHealth(Health - Value);
}
