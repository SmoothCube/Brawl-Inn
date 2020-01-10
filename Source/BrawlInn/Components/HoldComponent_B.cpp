// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldComponent_B.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"

#include "Items/Throwable_B.h"
#include "Player/PlayerCharacter_B.h"

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

bool UHoldComponent_B::TryPickup()
{
	if (!OwningPlayer) return false;
	if (HoldingItem) return false;
	if (ThrowableItemsInRange.Num() == 0) return false;

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
		return false;
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
	return true;
}

void UHoldComponent_B::Pickup(AThrowable_B* Item)
{
	Item->PickedUp();
	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
	Item->AttachToComponent(Cast<USceneComponent>(OwningPlayer->GetMesh()), rules, HoldingSocketName);
	HoldingItem = Item;
}

bool UHoldComponent_B::IsHolding()
{
	if (HoldingItem)
		return true;
	return false;
}

AThrowable_B* UHoldComponent_B::GetHoldingItem() const
{
	return HoldingItem;
}

void UHoldComponent_B::SetHoldingItem(AThrowable_B* Item)
{
	HoldingItem = Item;
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
