// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldComponent_B.h"
#include "Items/Throwable_B.h"
#include "Player/PlayerCharacter_B.h"
#include "Components/SkeletalMeshComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

UHoldComponent_B::UHoldComponent_B(const FObjectInitializer& ObjectInitializer)
{
	SphereRadius = PickupRange;
	PrimaryComponentTick.bCanEverTick = false;
}

void UHoldComponent_B::BeginPlay()
{
	Super::BeginPlay();
	SphereRadius = PickupRange;

	OnComponentBeginOverlap.AddDynamic(this, &UHoldComponent_B::AddItem);
	OnComponentEndOverlap.AddDynamic(this, &UHoldComponent_B::RemoveItem);

	OwningPlayer = Cast<APlayerCharacter_B>(GetOwner());
}

void UHoldComponent_B::TryPickup()
{
	if (!OwningPlayer) return;
	if (HoldingItem) return;
	if (DroppingItem) return;
	if (ThrowableItemsInRange.Num() == 0) return;

	FVector PlayerLocation = OwningPlayer->GetMesh()->GetComponentLocation();
	PlayerLocation.Z = 0;
	FVector PlayerForward = PlayerLocation + OwningPlayer->GetActorForwardVector() * PickupRange;
	PlayerForward.Z = 0;

	FVector PlayerToForward = PlayerForward - PlayerLocation;

	TArray<AThrowable_B*> ThrowableItemsInCone;

	for (const auto& Item : ThrowableItemsInRange)
	{
		if (Item->IsHeld()) continue;
		FVector ItemLocation = Item->GetActorLocation();
		ItemLocation.Z = 0;

		FVector PlayerToItem = ItemLocation - PlayerLocation;

		float AngleA = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(PlayerToForward.GetSafeNormal(), PlayerToItem.GetSafeNormal())));
		if (AngleA <= PickupAngle)
			ThrowableItemsInCone.Add(Item);
	}
	AThrowable_B* NearestItem = nullptr;

	switch (ThrowableItemsInCone.Num())
	{
	case 0:
		return;
	case 1:
		NearestItem = ThrowableItemsInCone[0];
		break;
	default:
		ThrowableItemsInCone.Sort([&](const AThrowable_B& LeftSide, const AThrowable_B& RightSide)
			{
				FVector A = LeftSide.GetActorLocation();
				A.Z = 0;
				FVector B = RightSide.GetActorLocation();
				B.Z = 0;
				float DistanceA = FVector::Dist(PlayerLocation, LeftSide.GetActorLocation());
				float DistanceB = FVector::Dist(PlayerLocation, RightSide.GetActorLocation());
				return DistanceA < DistanceB;
			});
		NearestItem = ThrowableItemsInCone[0];
		break;
	}
	Pickup(NearestItem);
}

void UHoldComponent_B::Pickup(AThrowable_B* Item)
{
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

//Called from the animation notify AnimNotify_PlayerDropThrowable_B
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
