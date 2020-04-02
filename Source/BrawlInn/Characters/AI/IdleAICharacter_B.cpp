// Fill out your copyright notice in the Description page of Project Settings.

#include "IdleAICharacter_B.h"


#include "AI/AIDropPoint_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "AIController_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/MergeMeshComponent_B.h"
#include "Hazards/Bar_B.h"

AIdleAICharacter_B::AIdleAICharacter_B()
{
	MergeMeshComponent = CreateDefaultSubobject<UMergeMeshComponent_B>("MergeComponent");
}

void AIdleAICharacter_B::BeginPlay()
{
	Super::BeginPlay();


	StartLocation = GetActorLocation();
	Bar = Cast<ABar_B>(UGameplayStatics::GetActorOfClass(GetWorld(), ABar_B::StaticClass()));
	bCanMove = false;
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "NPCPlatform", OutActors);
	if (OutActors.IsValidIndex(0))
		RespawnLocation = OutActors[0]->GetActorLocation() + FVector(0, 0, 150);
	else
		RespawnLocation = StartLocation;

	MergeMeshComponent->CreateRandomMesh();
	MergeMeshComponent->DestroyComponent();

	float Height = FMath::FRandRange(MinScaleValue, MaxScaleValue);
	float Width = FMath::FRandRange(MinScaleValue, MaxScaleValue);
	SetActorScale3D({ Width,  Width, Height });
}

void AIdleAICharacter_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AIdleAICharacter_B::FellOutOfWorld(const UDamageType& dmgType)
{
	Respawn();
}

void AIdleAICharacter_B::Respawn()
{
	bIsAlive = true;
	StandUp();
	SetActorLocation(RespawnLocation);
	bCanMove = false;
	GetWorld()->GetTimerManager().SetTimer(TH_ResetCanMove, [&]()
		{
			bCanMove = true;
			GetWorld()->GetTimerManager().ClearTimer(TH_Respawn);
			Cast<AAIController_B>(GetController())->OnCharacterFall().Broadcast();
		}, ResetCanMoveTime, false);
}

void AIdleAICharacter_B::Die()
{
	Super::Die();
	GetWorld()->GetTimerManager().SetTimer(TH_Respawn, [&]()
		{
			Respawn();
		}, 5, false);
}

void AIdleAICharacter_B::SetState(EState s)
{
	if (State == EState::EFallen && s == EState::EWalking)
		Cast<AAIController_B>(GetController())->OnCharacterFall().Broadcast();

	Super::SetState(s);
}

void AIdleAICharacter_B::OrderDrink()
{
	AAIDropPoint_B* DropPoint = GetWorld()->SpawnActor<AAIDropPoint_B>(BP_DropPoint, StartLocation, FRotator());
	DropPoint->OnItemDelivered().AddUObject(this, &AIdleAICharacter_B::TryPickup);
	Bar->GetOrder(DropPoint);
}

bool AIdleAICharacter_B::CanOrderDrink() const
{
	return bCanOrderDrink;
}

void AIdleAICharacter_B::TryPickup()
{
	HoldComponent->TryPickup();
}
