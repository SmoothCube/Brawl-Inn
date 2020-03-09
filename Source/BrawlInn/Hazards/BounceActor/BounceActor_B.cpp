// Fill out your copyright notice in the Description page of Project Settings.

#include "BounceActor_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"
#include "DestructibleComponent.h"
#include "Components/StaticMeshComponent.h"

#include "BrawlInn.h"
#include "Hazards/BounceActor/BarrelTargetPoint_B.h"
#include "System/GameModes/GameMode_B.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Characters/Player/PlayerController_B.h"
#include "Components/HoldComponent_B.h"
#include "System/DataTable_B.h"


ABounceActor_B::ABounceActor_B()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh->SetSimulatePhysics(true);
}

void ABounceActor_B::BeginPlay()
{
	Super::BeginPlay();
	ScoreAmount = UDataTable_B::CreateDataTable(FScoreTable::StaticStruct(), "DefaultScoreValues.csv")->GetRow<FScoreTable>("Barrel")->Value;

}

// Old Explode
void ABounceActor_B::OnItemFracture()
{
	Super::OnItemFracture();
	if (PlayerController)
		UGameplayStatics::ApplyRadialDamage(GetWorld(), ScoreAmount, GetActorLocation(), Radius, BP_DamageType, {}, this, PlayerController);
	else
		UGameplayStatics::ApplyRadialDamage(GetWorld(), ScoreAmount, GetActorLocation(), Radius, BP_DamageType, {}, this, PlayerController);
	
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(this);
	if (Interface && Interface->Execute_IsHeld(this))
	{
		if (OwningCharacter->HoldComponent->GetHoldingItem() == this)
		{
			OwningCharacter->HoldComponent->SetHoldingItem(nullptr);

			OwningCharacter->SetState(EState::EWalking);
		}
	}

	if (Target)
	{
		Target->SetActorHiddenInGame(true);
		if (bShouldDestroyTarget)
		{
			if (Target->Destroy())	//this doesnt work for some reason :c
				BWarn("Destroyed");
		}
	}

	SpawnPlayerCharacter();
}

void ABounceActor_B::SpawnPlayerCharacter()
{
	//Respawns player. Kinda hates having this here.
	if (PlayerController)
	{
		AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->SpawnCharacter_D.Broadcast(PlayerController->GetPlayerInfo(), true, FTransform(GetActorLocation()));
		}
	}
}
