// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "HoldComponent_B.generated.h"

class APlayerCharacter_B;

UCLASS()
class BRAWLINN_API UHoldComponent_B : public USphereComponent
{
	GENERATED_BODY()

public:
	UHoldComponent_B(const FObjectInitializer& ObjectInitializer);

	bool TryPickup();

	UFUNCTION(BlueprintPure)
	bool IsHolding();

	AActor* GetHoldingItem() const;
	void SetHoldingItem(AActor* Item);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	FName HoldingSocketName = FName("HoldingItemSocket");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PickupAngle = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PickupRange = 125.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InnerPickupRange = 25.f;

	UFUNCTION()
		void AddItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
		
	UFUNCTION()
		void RemoveItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);

public:
	void Drop();

private:

	void Pickup(AActor* Item);
	float PickupRadius = 0;
	TArray<AActor*> ThrowableItemsInRange;

	AActor* HoldingItem = nullptr;
	APlayerCharacter_B* OwningPlayer = nullptr;

};
