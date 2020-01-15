// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent_B.h"
#include "BrawlInn.h"
#include "TimerManager.h"
#include "Engine/World.h"

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


void UHealthComponent_B::BeginPlay()
{
	Super::BeginPlay();

	FireDamageStart_D.AddDynamic(this, &UHealthComponent_B::StartFireDamage);
	FireDamageStop_D.AddDynamic(this, &UHealthComponent_B::StopFireDamage);

}

void UHealthComponent_B::StartFireDamage()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(TH_IsOnFire))
		GetWorld()->GetTimerManager().SetTimer(TH_IsOnFire, this, &UHealthComponent_B::TakeFireDamage, FireTick, true);
	GetWorld()->GetTimerManager().SetTimer(TH_ResetFire, this, &UHealthComponent_B::ResetFireTimer, 5.f, false);
}

void UHealthComponent_B::StopFireDamage()
{
	GetWorld()->GetTimerManager().ClearTimer(TH_IsOnFire);
}

void UHealthComponent_B::TakeFireDamage()
{
	BWarn("Fire health: %f", FireHealthAmount);
	FireHealthAmount -= 1;

	if (FireHealthAmount <= 0)
		FireHealthIsZero_D.Broadcast();
}

int UHealthComponent_B::GetFireHealth() const
{
	return FireHealthAmount;
}

void UHealthComponent_B::ResetFireTimer()
{
	FireDamageStop_D.Broadcast();
}
