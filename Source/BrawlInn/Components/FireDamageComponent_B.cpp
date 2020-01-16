// Fill out your copyright notice in the Description page of Project Settings.

#include "FireDamageComponent_B.h"
#include "TimerManager.h"
#include "Engine/World.h"

#include "Player/PlayerController_B.h"
#include "Player/PlayerCharacter_B.h"
#include "Components/HealthComponent_B.h"

UFireDamageComponent_B::UFireDamageComponent_B()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFireDamageComponent_B::BeginPlay()
{
	Super::BeginPlay();

	FireHealthMaxAmount = FireHealthAmount;

	FireDamageStart_D.AddDynamic(this, &UFireDamageComponent_B::StartFireDamage);
	FireDamageStop_D.AddDynamic(this, &UFireDamageComponent_B::StopFireDamage);
	FireHealthIsZero_D.AddDynamic(this, &UFireDamageComponent_B::TakeDamage);
	FireHealthIsZero_D.AddDynamic(this, &UFireDamageComponent_B::StopFireDamage);
	
}

void UFireDamageComponent_B::TakeDamage()
{
	FireHealthAmount = FireHealthMaxAmount;
	Cast<APlayerCharacter_B>(GetOwner())->GetPlayerController_B()->HealthComponent->TakeDamage(1);
}

int UFireDamageComponent_B::GetFireHealth() const
{
	return FireHealthAmount;
}

void UFireDamageComponent_B::StartFireDamage()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(TH_IsOnFire))
	{
		GetWorld()->GetTimerManager().SetTimer(TH_IsOnFire, this, &UFireDamageComponent_B::TakeFireDamage, FireTick, true);
	}
	GetWorld()->GetTimerManager().SetTimer(TH_ResetFire, this, &UFireDamageComponent_B::ResetFireTimer, FireStopTimer, false);
}

void UFireDamageComponent_B::StopFireDamage()
{
	GetWorld()->GetTimerManager().ClearTimer(TH_IsOnFire);
}

void UFireDamageComponent_B::TakeFireDamage()
{
	FireHealthAmount -= FireDamagePerTick;

	if (FireHealthAmount <= 0)
		FireHealthIsZero_D.Broadcast();
}

void UFireDamageComponent_B::ResetFireTimer()
{
	FireDamageStop_D.Broadcast();
}
