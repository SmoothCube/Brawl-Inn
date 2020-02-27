// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Throwable_B.h"
#include "DestrutibleItem_B.generated.h"

class UDestructibleComponent_B;

UCLASS()
class BRAWLINN_API ADestrutibleItem_B : public AItem_B
{
	GENERATED_BODY()
public:
	ADestrutibleItem_B();

	UPROPERTY(VisibleAnywhere)
		UDestructibleComponent_B* DestructibleComponent;

	virtual void BeginPlay() override;

	UFUNCTION()
		void OnFracture(const FVector& HitPoint, const FVector& HitDirection);

	virtual void PickedUp_Implementation(ACharacter_B* Player) override;

	virtual void Dropped_Implementation() override;

	virtual void Use_Implementation() override;

	UPROPERTY(EditAnywhere, Category = "Variables")
		float ThrowHitStrength = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Variables")
		int DamageAmount = 1;

	virtual void OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
