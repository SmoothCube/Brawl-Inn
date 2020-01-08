// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldComponent_B.h"
#include "Items/Throwable_B.h"
#include "Player/PlayerCharacter_B.h"

UHoldComponent_B::UHoldComponent_B(const FObjectInitializer& ObjectInitializer)
{
	SphereRadius = 150.f;
}

void UHoldComponent_B::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UHoldComponent_B::AddItem);
	OnComponentEndOverlap.AddDynamic(this, &UHoldComponent_B::RemoveItem);

	OwningPlayer = Cast<APlayerCharacter_B>(GetOwner());
}

void UHoldComponent_B::TryPickup()
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *GetNameSafe(OwningPlayer));
	if (!OwningPlayer) return;
	if (HoldingItem) return;
	if (DroppingItem) return;
	if (ThrowableItemsInRange.Num() == 0) return;

	ThrowableItemsInRange.Sort([&](const AThrowable_B& LeftSide, const AThrowable_B& RightSide)
		{
			float DistanceA = FVector::Dist(GetComponentLocation(), LeftSide.GetActorLocation());
			float DistanceB = FVector::Dist(GetComponentLocation(), RightSide.GetActorLocation());
			return DistanceA < DistanceB;
		});

	AThrowable_B* NearestItem = ThrowableItemsInRange[0];
	
	if (NearestItem->IsHeld()) return;

	Pickup(NearestItem);
}

void UHoldComponent_B::Pickup(AThrowable_B* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *GetNameSafe(Item));
	Item->PickedUp();
	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
	Item->AttachToComponent(Cast<USceneComponent>(OwningPlayer->GetMesh()), rules, HoldingSocketName);

	HoldingItem = Item;
}
void UHoldComponent_B::TryDrop()
{
	if (!OwningPlayer) return;
	if (!HoldingItem) return;
	
	InitDrop();
}
bool UHoldComponent_B::IsHolding()
{
	if (HoldingItem)
		return true;
	return false;
}

void UHoldComponent_B::InitDrop()
{
	DroppingItem = HoldingItem;
	HoldingItem = nullptr;
}
void UHoldComponent_B::Drop()
{
	if (!DroppingItem)
		return;

	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DroppingItem->DetachFromActor(rules);
	DroppingItem->Dropped();
	DroppingItem = nullptr;
}

void UHoldComponent_B::AddItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AThrowable_B* Item = Cast<AThrowable_B>(OtherActor);
	if (!IsValid(Item))
		return;

	ThrowableItemsInRange.Add(Item);
}

void UHoldComponent_B::RemoveItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	AThrowable_B* Item = Cast<AThrowable_B>(OtherActor);
	if (!IsValid(Item))
		return;

	ThrowableItemsInRange.Remove(Item);
}
