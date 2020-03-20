// Fill out your copyright notice in the Description page of Project Settings.

#include "HoldComponent_B.h"
#include "Components/SkeletalMeshComponent.h"

#include "BrawlInn.h"
#include "Characters/Character_B.h"
#include "Components/PunchComponent_B.h"
#include "Items/Item_B.h"
#include "Items/Useable_B.h"

UHoldComponent_B::UHoldComponent_B(const FObjectInitializer& ObjectInitializer)
{
	SphereRadius = PickupRange;
	PrimaryComponentTick.bCanEverTick = false;
	SetCollisionProfileName("Pickup-Trigger");
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
	if (OwningCharacter->PunchComponent->GetIsPunching()) return false;
	BWarn("Trying Pickup!");
	TArray<AActor*> OverlappingThrowables;
	GetOverlappingActors(OverlappingThrowables, UThrowableInterface_B::StaticClass());
	OverlappingThrowables.Remove(OwningCharacter);

	if (OverlappingThrowables.Num() == 0)
		return false;

	FVector PlayerLocation = OwningCharacter->GetMesh()->GetComponentLocation();
	PlayerLocation.Z = 0;
	FVector PlayerForward = PlayerLocation + OwningCharacter->GetActorForwardVector() * PickupRange;
	PlayerForward.Z = 0;

	FVector PlayerToForward = PlayerForward - PlayerLocation;

	TArray<AActor*> ThrowableItemsInCone;
	for (const auto& Item : OverlappingThrowables)
	{
		if (!IsValid(Item) || Item == OwningCharacter)
			continue;

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

	switch (ThrowableItemsInCone.Num())
	{
	case 0:
		OverlappingThrowables.Sort([&](const AActor& LeftSide, const AActor& RightSide)
			{
				FVector A = LeftSide.GetActorLocation();
				A.Z = 0;
				FVector B = RightSide.GetActorLocation();
				B.Z = 0;
				float DistanceA = FVector::Dist(PlayerLocation, LeftSide.GetActorLocation());
				float DistanceB = FVector::Dist(PlayerLocation, RightSide.GetActorLocation());
				return DistanceA < DistanceB;
			});
		NearestItem = OverlappingThrowables[0];
		break;
	case 1:
		NearestItem = ThrowableItemsInCone[0];	//crashed?
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
	ACharacter_B* Character = Cast<ACharacter_B>(NearestItem);
	if (Character)
	{
		if (Character->CanBeHeld_Implementation() && !Character->IsInvulnerable())
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
	OwningCharacter->SetState(EState::EHolding);
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

	if (Character)
	{
		Character->AddActorLocalOffset(FVector(0, 0, 75));
		Character->SetActorRelativeRotation(Character->GetHoldRotation());
	}
	else
	{
		Item->AddActorLocalOffset(Offset);
		Item->SetActorRelativeRotation(Cast<AItem_B>(Item)->GetHoldRotation());
	}
}

void UHoldComponent_B::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == OwningCharacter) return;
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(OtherActor);
	if (!Interface || !Interface->Execute_CanBeHeld(OtherActor))
		return;
}

void UHoldComponent_B::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	if (OtherActor == OwningCharacter) return;
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(OtherActor);
	if (!Interface)
		return;

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
	if (IsValid(OwningCharacter))
		OwningCharacter->SetState(EState::EWalking);
}