// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent_B.h"
#include "BrawlInn.h"
#include "UI/Game/HealthWidget_B.h"
#include "Kismet/GameplayStatics.h"

#include "Characters/Player/PlayerController_B.h"
#include "System/GameModes/GameMode_B.h"


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

		HealthIsZero_D.Broadcast();
		AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			APlayerController_B* PlayerController = Cast<APlayerController_B>(GetOwner());
			if (GetRespawns() <= 0)
			{
				RespawnIsZero_D.Broadcast();
				GameMode->DespawnCharacter_D.Broadcast(PlayerController, false);
			}
			else
			{
				GameMode->DespawnCharacter_D.Broadcast(PlayerController, true);
			}
		}
	}
	HealthUpdated_D.Broadcast(Health);
}

void UHealthComponent_B::TakeDamage(int Value)
{
	SetHealth(Health - Value);
}

void UHealthComponent_B::SetHealthWidget(UHealthWidget_B* Widget)
{
	HealthWidget = Widget;
	if (!HealthUpdated_D.IsBoundToObject(Widget))
		HealthUpdated_D.AddUObject(Widget, &UHealthWidget_B::UpdateHealthAmount);

	if (!HealthIsZero_D.IsBoundToObject(Widget))
		HealthIsZero_D.AddUObject(Widget, &UHealthWidget_B::UpdateRespawnsAmount);

}

void UHealthComponent_B::BeginPlay()
{
	Super::BeginPlay();

	StartingHealth = Health;
}
