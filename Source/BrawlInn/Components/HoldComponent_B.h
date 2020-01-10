// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "HoldComponent_B.generated.h"

class AThrowable_B;
class APlayerCharacter_B;

UCLASS()
class BRAWLINN_API UHoldComponent_B : public USphereComponent
{
	GENERATED_BODY()

public:

	UHoldComponent_B(const FObjectInitializer& ObjectInitializer);

	bool TryPickup();
	void TryDrop();
	void Drop();

	UFUNCTION(BlueprintPure)
	bool IsHolding();

	AThrowable_B* GetHoldingItem() const;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	FName HoldingSocketName = FName("HoldingItemSocket");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PickupAngle = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PickupRange = 100;

private:

	AThrowable_B* HoldingItem = nullptr;
	AThrowable_B* DroppingItem = nullptr; //Flytte dette til Throw?

	TArray<AThrowable_B*> ThrowableItemsInRange;

	void Pickup(AThrowable_B* Item);
	void InitDrop();

	float PickupRadius = 0;
	
	UFUNCTION()
	void AddItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void RemoveItem(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);

	APlayerCharacter_B* OwningPlayer = nullptr;
};
