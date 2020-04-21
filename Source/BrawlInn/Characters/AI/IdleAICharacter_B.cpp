// Fill out your copyright notice in the Description page of Project Settings.

#include "IdleAICharacter_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "AI/AIDropPoint_B.h"
#include "AIController_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/MergeMeshComponent_B.h"
#include "Hazards/Bar_B.h"
#include "System/GameModes/MainGameMode_B.h"


AIdleAICharacter_B::AIdleAICharacter_B()
{
	MergeMeshComponent = CreateDefaultSubobject<UMergeMeshComponent_B>("MergeComponent");
}

void AIdleAICharacter_B::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		Bar = GameMode->GetBar();
	}
	bCanMove = false;
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "NPCPlatform", OutActors);
	if (OutActors.IsValidIndex(0))
		RespawnLocation = OutActors[0]->GetActorLocation() + FVector(0, 0, 150);
	else
		RespawnLocation = StartLocation;

	if (MergeMeshComponent && MergeMeshComponent->bRandomizeOnBeginPlay)
	{
		MergeMeshComponent->CreateRandomMesh(GetMesh());
		MergeMeshComponent->DestroyComponent();
	}
}

void AIdleAICharacter_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AIdleAICharacter_B::FellOutOfWorld(const UDamageType& dmgType)
{
	Die();
}

void AIdleAICharacter_B::Respawn()
{
	bIsAlive = true;
	StandUp();
	SetActorLocation(RespawnLocation);
	bCanMove = false;
	Cast<AAIController_B>(GetController())->OnCharacterFall().Broadcast();
}

void AIdleAICharacter_B::Die()
{
	Super::Die();
	GetWorld()->GetTimerManager().SetTimer(TH_Respawn, this, &AIdleAICharacter_B::Respawn, 4, false);
}

void AIdleAICharacter_B::SetState(const EState StateIn)
{
	if (StateIn == EState::EFallen && StateIn == EState::EWalking)
		Cast<AAIController_B>(GetController())->OnCharacterFall().Broadcast();

	Super::SetState(StateIn);
}

void AIdleAICharacter_B::OrderDrink()
{
	if (Bar)
	{
		AAIDropPoint_B* DropPoint = GetWorld()->SpawnActor<AAIDropPoint_B>(BP_DropPoint, StartLocation, FRotator());
		DropPoint->OnItemDelivered().AddUObject(this, &AIdleAICharacter_B::TryPickup);
		Bar->GetOrder(DropPoint);
	}
}

bool AIdleAICharacter_B::CanOrderDrink() const
{
	return bCanOrderDrink;
}

void AIdleAICharacter_B::TryPickup()
{
	HoldComponent->TryPickup();
}
