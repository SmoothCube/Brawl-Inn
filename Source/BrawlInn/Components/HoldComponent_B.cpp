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
	PrimaryComponentTick.bCanEverTick = true;

}

void UHoldComponent_B::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UHoldComponent_B::AddItem);
	OnComponentEndOverlap.AddDynamic(this, &UHoldComponent_B::RemoveItem);

	OwningPlayer = Cast<APlayerCharacter_B>(GetOwner());

	CalculatePickupRadius();

}

void UHoldComponent_B::CalculatePickupRadius()
{
	// ESPEN SKJØNNER IKKE MATTE - Espen 2020 --- NVM, ESPEN SKJØNNER DET LIKEVEL
	float Angle2 = 90;
	float Angle3 = 180 - (PickupAngle + Angle2);

	PickupRadius = (PickupRange * FMath::Sin(FMath::DegreesToRadians(PickupAngle))) / FMath::Sin(FMath::DegreesToRadians(Angle3));
	UE_LOG(LogTemp, Warning, TEXT("Radius: %f"), PickupRadius);
}

void UHoldComponent_B::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

FVector UHoldComponent_B::AimLocation(bool& Hit)
{
	//if (ThrowableItemsInRange.Num() == 0)
	//{
	//	Hit = false;
	//	return FVector(0);
	//}

	//for (const auto& Item : ThrowableItemsInRange)
	//{
	//	FVector Point = Item->GetActorLocation();
	//	FVector StartPoint = OwningPlayer->GetMesh()->GetComponentLocation();

	//	Point.Z = StartPoint.Z;

	//	float Out;
	//	bool hit = FMath::GetDistanceWithinConeSegment(Point, StartPoint, OwningPlayer->GetActorForwardVector() * Range, 0, ThrowRadius, Out);

	//	if (hit)
	//		UE_LOG(LogTemp, Warning, TEXT("HIT %f"), Out)
	//	else
	//		UE_LOG(LogTemp, Warning, TEXT("NO HIT %f"), Out)
	//}
	return FVector(0);
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

	FVector a = PlayerForward - PlayerLocation;

	TArray<AThrowable_B*> ThrowableItemsInCone;

	for (const auto& Item : ThrowableItemsInRange)
	{
		if (Item->IsHeld()) continue;
		FVector ItemLocation = Item->GetActorLocation();
		ItemLocation.Z = 0;

		FVector b = ItemLocation - PlayerLocation;

		float AngleA = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(a.GetSafeNormal(), b.GetSafeNormal())));
		UE_LOG(LogTemp, Warning, TEXT("AngleA %f"), AngleA);
		if (AngleA <= PickupAngle)
		{
			ThrowableItemsInCone.Add(Item);
		}
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

	UE_LOG(LogTemp, Warning, TEXT("Item added %s"), *GetNameSafe(Item));
	ThrowableItemsInRange.Add(Item);
}

void UHoldComponent_B::RemoveItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	AThrowable_B* Item = Cast<AThrowable_B>(OtherActor);
	if (!IsValid(Item))
		return;

	UE_LOG(LogTemp, Warning, TEXT("Item removed %s"), *GetNameSafe(Item));
	ThrowableItemsInRange.Remove(Item);
}
