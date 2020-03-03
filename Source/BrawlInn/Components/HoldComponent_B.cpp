// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldComponent_B.h"
//#include "Math/UnrealMathUtility.h"
//#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"
//
#include "BrawlInn.h"
#include "Components/PunchComponent_B.h"
//#include "System/Interfaces/ThrowableInterface_B.h"
//#include "Items/Throwable_B.h"
#include "Characters/Character_B.h"

UHoldComponent_B::UHoldComponent_B(const FObjectInitializer& ObjectInitializer)
{
	SphereRadius = PickupRange;
	PrimaryComponentTick.bCanEverTick = false;
}

void UHoldComponent_B::BeginPlay()
{
	Super::BeginPlay();
	SphereRadius = PickupRange;

	OnComponentBeginOverlap.AddDynamic(this, &UHoldComponent_B::OnOverlapBegin);
	OnComponentEndOverlap.AddDynamic(this, &UHoldComponent_B::OnOverlapEnd);

	OwningCharacter = Cast<ACharacter_B>(GetOwner());
}

bool UHoldComponent_B::TryPickup()
{
	if (!OwningCharacter || OwningCharacter->GetState() != EState::EWalking) return false;
	if (HoldingItem || ThrowableItemsInRange.Num() == 0) return false;
	if (OwningCharacter->PunchComponent->bIsPunching) return false;

	FVector PlayerLocation = OwningCharacter->GetMesh()->GetComponentLocation();
	PlayerLocation.Z = 0;
	FVector PlayerForward = PlayerLocation + OwningCharacter->GetActorForwardVector() * PickupRange;
	PlayerForward.Z = 0;

	FVector PlayerToForward = PlayerForward - PlayerLocation;

	TArray<AActor*> ThrowableItemsInCone;

	TArray<AActor*> BrokenItems;
	for (const auto& Item : ThrowableItemsInRange)
	{
		if (!IsValid(Item))
		{
			BrokenItems.Add(Item);
			continue;
		}
		IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(Item);
		if (!Interface) continue;

		if (Interface->Execute_IsHeld(Item)) continue; //Had a crash here before adding cleanup

		FVector ItemLocation = Item->GetActorLocation();
		ItemLocation.Z = 0;

		FVector PlayerToItem = ItemLocation - PlayerLocation;

		float AngleA = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(PlayerToForward.GetSafeNormal(), PlayerToItem.GetSafeNormal())));
		if (AngleA <= PickupAngle)
			ThrowableItemsInCone.Add(Item);
	}
	AActor* NearestItem = nullptr;
	for (auto& brokenItem : BrokenItems)
	{
		ThrowableItemsInRange.Remove(brokenItem);
	}
	BrokenItems.Empty();
	if (ThrowableItemsInRange.Num() == 0) return false;

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
	ACharacter_B* Player = Cast<ACharacter_B>(NearestItem);						//TODO gjør om
	if (Player)
	{
		if ((Player->GetState() == EState::EFallen) && !Player->IsInvulnerable())
		{
			Pickup(NearestItem);
			return true;
		}
		return false;
	}

	Pickup(NearestItem);
	return true;
}

void UHoldComponent_B::AddItem(AActor* ActorToAdd)
{
	auto Index = ThrowableItemsInRange.Find(ActorToAdd); //Only add if it doesnt exist.
	if (Index == INDEX_NONE)
		ThrowableItemsInRange.Add(ActorToAdd);
}

void UHoldComponent_B::RemoveItem(AActor* ActorToRemove)
{
	ThrowableItemsInRange.Remove(ActorToRemove);
}

void UHoldComponent_B::Pickup(AActor* Item)
{
	OwningCharacter->SetState(EState::EHolding);

	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(Item);
	if (Interface)
		Interface->Execute_PickedUp(Item, OwningCharacter);

	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	Item->AttachToComponent(Cast<USceneComponent>(OwningCharacter->GetMesh()), rules, HoldingSocketName);
	HoldingItem = Item;
}

void UHoldComponent_B::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == OwningCharacter) return;
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(OtherActor);
	if (!Interface || !Interface->Execute_CanBeHeld(OtherActor))
		return;

	AddItem(OtherActor);
}

void UHoldComponent_B::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	if (OtherActor == OwningCharacter) return;
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(OtherActor);
	if (!Interface)
		return;

	RemoveItem(OtherActor);
}

bool UHoldComponent_B::IsHolding()
{
	return (HoldingItem ? true : false);
}

AActor* UHoldComponent_B::GetHoldingItem() const
{
	return HoldingItem;
}

void UHoldComponent_B::SetHoldingItem(AActor* Item)
{
	HoldingItem = Item;
}

void UHoldComponent_B::Drop()
{
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(GetHoldingItem());
	if (Interface)
	{
		Interface->Execute_Dropped(GetHoldingItem());
	}
	SetHoldingItem(nullptr);
	OwningCharacter->SetState(EState::EWalking);
}