// Fill out your copyright notice in the Description page of Project Settings.

#include "IdleAICharacter_B.h"

#include "AIController_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void AIdleAICharacter_B::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "NPCPlatform", OutActors);
	if (OutActors.IsValidIndex(0))
		RespawnLocation = OutActors[0]->GetActorLocation() + FVector(0, 0, 150);
	else
		RespawnLocation = StartLocation;
}

void AIdleAICharacter_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AIdleAICharacter_B::FellOutOfWorld(const UDamageType& dmgType)
{
	SetActorLocation(RespawnLocation);
	bCanMove = false;
	GetWorld()->GetTimerManager().SetTimer(TH_ResetCanMove, [&]()
		{
			bCanMove = true;
			Cast<AAIController_B>(GetController())->OnCharacterFall().Broadcast();
		}, ResetCanMoveTime, false);
}

void AIdleAICharacter_B::SetState(EState s)
{
	if (State == EState::EFallen && s == EState::EWalking)
		Cast<AAIController_B>(GetController())->OnCharacterFall().Broadcast();

	Super::SetState(s);
}
