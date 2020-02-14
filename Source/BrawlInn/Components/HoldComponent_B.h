// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "HoldComponent_B.generated.h"

class ACharacter_B;

UCLASS()
class BRAWLINN_API UHoldComponent_B : public USphereComponent
{
	GENERATED_BODY()

public:
	UHoldComponent_B(const FObjectInitializer& ObjectInitializer);

	// ********** UActorComponent **********

protected:
	virtual void BeginPlay() override;

	// ********** Pickup **********
public:
	bool TryPickup();

	void AddItem(AActor* ActorToAdd);

	void RemoveItem(AActor* ActorToRemove);

protected:
	void Pickup(AActor* Item);

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PickupAngle = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PickupRange = 125.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InnerPickupRange = 25.f;

	UPROPERTY()
		TArray<AActor*> ThrowableItemsInRange;

	// ********** Holding **********
public:
	bool IsHolding();

	AActor* GetHoldingItem() const;

	void SetHoldingItem(AActor* Item);
protected:

	UPROPERTY()
		AActor* HoldingItem = nullptr;

	UPROPERTY(EditAnywhere)
		FName HoldingSocketName = FName("HoldingItemSocket");

	// ********** Misc. **********
public:
	void Drop();

protected:
	UPROPERTY()
		ACharacter_B* OwningCharacter = nullptr;

};
