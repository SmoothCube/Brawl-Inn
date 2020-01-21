// Fill out your copyright notice in the Description page of Project Settings.


#include "AOEDamageArea_B.h"
#include "Player/PlayerCharacter_B.h"
#include "BrawlInn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"

AAOEDamageArea_B::AAOEDamageArea_B()
{
	SetActorTickInterval(1);
	SetActorTickEnabled(false);
}

void AAOEDamageArea_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> Players;
	GetOverlappingActors(Players, APlayerCharacter_B::StaticClass());

	for (const auto& Player : Players)
	{
		BLog("Fire Active");
		UGameplayStatics::ApplyDamage(Player, 1, Cast<APlayerCharacter_B>(Player)->GetController(), this, AOEDamageType);
	}
}

void AAOEDamageArea_B::ActivateFire()
{
	SetActorTickEnabled(true);
	if (FireActivated_D.IsBound())
		FireActivated_D.Execute(this);
}

void AAOEDamageArea_B::DeactivateFire()
{
	//SetActorTickEnabled(false);
	if (FireDeactivated_D.IsBound())
		FireDeactivated_D.Execute(this);
}

