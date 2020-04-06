// Fill out your copyright notice in the Description page of Project Settings.

#include "HoldComponent_B.h"
#include "Components/SkeletalMeshComponent.h"

#include "Characters/Character_B.h"
#include "Components/PunchComponent_B.h"
#include "Items/Useable_B.h"

UHoldComponent_B::UHoldComponent_B(const FObjectInitializer& ObjectInitializer)
{
	SphereRadius = 200.f;
	PrimaryComponentTick.bCanEverTick = false;
}

void UHoldComponent_B::BeginPlay()
{
	Super::BeginPlay();

//	SetCollisionProfileName("Pickup-Trigger");

	OwningCharacter = Cast<ACharacter_B>(GetOwner());
}

bool UHoldComponent_B::TryPickup()
{
	check(IsValid(OwningCharacter));

	if (OwningCharacter->GetState() != EState::EWalking || OwningCharacter->PunchComponent->GetIsPunching()) return false;

	TArray<AActor*> OverlappingThrowables;
	GetOverlappingActors(OverlappingThrowables, UThrowableInterface_B::StaticClass());
	OverlappingThrowables.Remove(OwningCharacter);

	if (OverlappingThrowables.Num() == 0)
		return false;

	FVector PlayerLocation = OwningCharacter->GetMesh()->GetComponentLocation();
	PlayerLocation.Z = 0;
	FVector PlayerForward = PlayerLocation + OwningCharacter->GetActorForwardVector() * SphereRadius;
	PlayerForward.Z = 0;

	const FVector PlayerToForward = PlayerForward - PlayerLocation;

	TArray<AActor*> ThrowableItemsInCone;
	for (const auto& Item : OverlappingThrowables)
	{
		if (!IsValid(Item) || Item == OwningCharacter)
			continue;

		IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(Item);
		if (!Interface || Interface->Execute_IsHeld(Item)) continue;

		FVector ItemLocation = Item->GetActorLocation();
		ItemLocation.Z = 0;

		FVector PlayerToItem = ItemLocation - PlayerLocation;

		const float AngleA = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(PlayerToForward.GetSafeNormal(), PlayerToItem.GetSafeNormal())));
		if (AngleA <= PickupAngle)
			ThrowableItemsInCone.Add(Item);
	}
	AActor* NearestItem = nullptr;

	switch (ThrowableItemsInCone.Num())
	{
	case 0:
		// Zero items in cone, picking up a item in the sphere instead.
		OverlappingThrowables.Sort([&](const AActor& LeftSide, const AActor& RightSide)
			{
				const IThrowableInterface_B* Interface1 = Cast<IThrowableInterface_B>(&LeftSide);
				const IThrowableInterface_B* Interface2 = Cast<IThrowableInterface_B>(&LeftSide);

				const float ValueA = Interface1->Execute_GetPickupWeight(&LeftSide);
				const float ValueB = Interface2->Execute_GetPickupWeight(&LeftSide);
				return ValueA > ValueB;
			});
		NearestItem = OverlappingThrowables[0];
		break;
	case 1:
		// One item in the cone, picking it up.
		NearestItem = ThrowableItemsInCone[0];
		break;
	default:
		// Two or more item in the cone, sorting them and picks up the highest value.
		ThrowableItemsInCone.Sort([&](const AActor& LeftSide, const AActor& RightSide)	//TODO Ooptimize?
			{
				const IThrowableInterface_B* Interface1 = Cast<IThrowableInterface_B>(&LeftSide);
				const IThrowableInterface_B* Interface2 = Cast<IThrowableInterface_B>(&LeftSide);

				const float ValueA = Interface1->Execute_GetPickupWeight(&LeftSide);
				const float ValueB = Interface2->Execute_GetPickupWeight(&LeftSide);
				return ValueA > ValueB;
			});
		if (ThrowableItemsInCone.IsValidIndex(0))
			NearestItem = ThrowableItemsInCone[0];
		break;
	}

	ACharacter_B* Character = Cast<ACharacter_B>(NearestItem);
	if (Character)
	{
		if (Character->Execute_CanBeHeld(Character) && !Character->IsInvulnerable())
		{
			Pickup(NearestItem);
			return true;
		}
		return false;
	}

	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(NearestItem);
	if (Interface->Execute_CanBeHeld(NearestItem))
	{
		Pickup(NearestItem);
		return true;
	}
	return false;
}

void UHoldComponent_B::Pickup(AActor* Item)
{
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(Item);
	if (Interface)
		Interface->Execute_PickedUp(Item, OwningCharacter);

	FVector Offset = FVector::ZeroVector;
	FName HoldSocketName = HoldingSocketName;
	if (Item->IsA(AUseable_B::StaticClass()))
	{
		Offset = OwningCharacter->HoldingDrinkOffset;
		HoldSocketName = FName("HoldingDrinkSocket");
	}
	const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	Item->AttachToComponent(Cast<USceneComponent>(OwningCharacter->GetMesh()), Rules, HoldSocketName);
	HoldingItem = Item;

	ACharacter_B* Character = Cast<ACharacter_B>(Item);

	Item->AddActorLocalOffset(Character ? FVector(0, 0, 75) : Offset);
	Item->SetActorRelativeRotation(Character ? Character->GetHoldRotation() : Cast<AItem_B>(Item)->GetHoldRotation());

	OwningCharacter->SetState(EState::EHolding);
}

bool UHoldComponent_B::IsHolding() const
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
		Interface->Execute_Dropped(GetHoldingItem());

	SetHoldingItem(nullptr);
	if (IsValid(OwningCharacter))
		OwningCharacter->SetState(EState::EWalking);
}