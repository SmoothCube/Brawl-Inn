// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent_B.h"
#include "BrawlInn.h"
#include "UI/Game/HealthWidget_B.h"


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

void UHealthComponent_B::SetHealthWidget(UHealthWidget_B* Widget)
{
	HealthWidget = Widget;
	if (!HealthUpdated_D.IsAlreadyBound(Widget, &UHealthWidget_B::UpdateHealthAmount))
		HealthUpdated_D.AddDynamic(Widget, &UHealthWidget_B::UpdateHealthAmount);
}
