// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldComponent_B.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"

#include "Components/PunchComponent_B.h"
#include "System/Interfaces/ThrowableInterface_B.h"
#include "Items/Throwable_B.h"
#include "Player/PlayerCharacter_B.h"
#include "BrawlInn.h"

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
	if (!(OwningPlayer->GetState() == EState::EWalking)) return false;
	if (HoldingItem) return false;
	if (ThrowableItemsInRange.Num() == 0) return false;
	if (OwningPlayer->PunchComponent->bIsPunching) return false;
	//Should only be able

	FVector PlayerLocation = OwningPlayer->GetMesh()->GetComponentLocation();
	PlayerLocation.Z = 0;
	FVector PlayerForward = PlayerLocation + OwningPlayer->GetActorForwardVector() * PickupRange;
	PlayerForward.Z = 0;

	FVector PlayerToForward = PlayerForward - PlayerLocation;

	TArray<AActor*> ThrowableItemsInCone;

	for (const auto& Item : ThrowableItemsInRange)
	{
		IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(Item);
		if (!Interface) continue;
		
		if(Interface->Execute_IsHeld(Item)) continue;
		
		FVector ItemLocation = Item->GetActorLocation();
		ItemLocation.Z = 0;

		FVector PlayerToItem = ItemLocation - PlayerLocation;

		float AngleA = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(PlayerToForward.GetSafeNormal(), PlayerToItem.GetSafeNormal())));
		if (AngleA <= PickupAngle)
			ThrowableItemsInCone.Add(Item);
	}
	AActor* NearestItem = nullptr;

	switch (ThrowableItemsInCone.Num())
	{
	case 0:
		ThrowableItemsInRange.Sort([&](const AActor& LeftSide, const AActor& RightSide)
			{
				FVector A = LeftSide.GetActorLocation();
				A.Z = 0;
				FVector B = RightSide.GetActorLocation();
				B.Z = 0;
				float DistanceA = FVector::Dist(PlayerLocation, LeftSide.GetActorLocation());
				float DistanceB = FVector::Dist(PlayerLocation, RightSide.GetActorLocation());
				return DistanceA < DistanceB;
			});
		NearestItem = ThrowableItemsInRange[0];
		break;
	case 1:
		NearestItem = ThrowableItemsInCone[0];
		break;
	default:
		ThrowableItemsInCone.Sort([&](const AActor& LeftSide, const AActor& RightSide)
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
	APlayerCharacter_B* Player = Cast<APlayerCharacter_B>(NearestItem);
	if (Player)
	{
		if ((Player->GetState() == EState::EFallen || Player->GetState() == EState::EStunned) && !Player->IsInvulnerable())
		{
			Pickup(NearestItem);
			return true;
		}
		return false;
	}

	Pickup(NearestItem);
	return true;
}

void UHoldComponent_B::Pickup(AActor* Item)
{
	OwningPlayer->SetState(EState::EHolding);

	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(Item);
	if (Interface)
	{
		Interface->Execute_PickedUp(Item, OwningPlayer);
	}

	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	Item->AttachToComponent(Cast<USceneComponent>(OwningPlayer->GetMesh()), rules, HoldingSocketName);
	HoldingItem = Item;


}

bool UHoldComponent_B::IsHolding()
{
	if (HoldingItem)
		return true;
	return false;
}

AActor* UHoldComponent_B::GetHoldingItem() const
{
	return HoldingItem;
}

void UHoldComponent_B::SetHoldingItem(AActor* Item)
{
	HoldingItem = Item;
}

void UHoldComponent_B::AddItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == OwningPlayer) return;
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(OtherActor);
	if (!Interface)
		return;
	ThrowableItemsInRange.Add(OtherActor);
	BWarn("%s Overlapping with: %s", *GetNameSafe(this), *GetNameSafe(OtherActor));
}

void UHoldComponent_B::RemoveItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	if (OtherActor == OwningPlayer) return;
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(OtherActor);
	if (!Interface)
		return;
	ThrowableItemsInRange.Remove(OtherActor);
}

void UHoldComponent_B::Drop()
{
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(GetHoldingItem());
	if (Interface)
	{
		Interface->Execute_Dropped(GetHoldingItem());
	}
	SetHoldingItem(nullptr);
}