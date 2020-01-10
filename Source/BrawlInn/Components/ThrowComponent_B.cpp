// Fill out your copyright notice in the Description page of Project Settings.

#include "ThrowComponent_B.h"
#include "BrawlInn.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

#include "Player/PlayerCharacter_B.h"
#include "Components/HoldComponent_B.h"
#include "Items/Throwable_B.h"
#include "System/GameMode_B.h"

UThrowComponent_B::UThrowComponent_B(const FObjectInitializer& ObjectInitializer)
{
	SphereRadius = 400;
	ShapeColor = FColor::Blue;
	PrimaryComponentTick.bCanEverTick = false;
}

bool UThrowComponent_B::TryThrow()
{
	if (!IsReady() || !HoldComponent->IsHolding())
		return false;

	bIsThrowing = true;
	return true;
}

bool UThrowComponent_B::IsThrowing() const
{
	return bIsThrowing;
}

void UThrowComponent_B::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UThrowComponent_B::BeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UThrowComponent_B::EndOverlap);

	OwningPlayer = Cast<APlayerCharacter_B>(GetOwner());

	GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->GameModeInit_D.AddDynamic(this, &UThrowComponent_B::GameIsReady);
}

void UThrowComponent_B::GameIsReady()
{
	/// Finds all characters
	TArray<AActor*> TempArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter_B::StaticClass(), TempArray);
	for (const auto& Player : TempArray)
		OtherPlayers.Add(Cast<APlayerCharacter_B>(Player));

	/// Finds the holdcomponent.
	HoldComponent = OwningPlayer->HoldComponent;
}

void UThrowComponent_B::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void UThrowComponent_B::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
}

void UThrowComponent_B::Throw()
{
		if (!HoldComponent->IsHolding())
			return;

	BScreen("Throwing")
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	HoldComponent->GetHoldingItem()->DetachFromActor(rules);
	HoldComponent->GetHoldingItem()->Dropped();
	HoldComponent->GetHoldingItem()->Mesh->AddImpulse(OwningPlayer->GetActorForwardVector() * ImpulseSpeed, NAME_None, true);
	HoldComponent->SetHoldingItem(nullptr);
	bIsThrowing = false;
}

bool UThrowComponent_B::IsReady() const
{
	if (GameMode && OwningPlayer && HoldComponent)
		return true;
	return false;
}
