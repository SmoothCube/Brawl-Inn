// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item_B.h"
#include "Throwable_B.generated.h"

class UDestructibleComponent;

UCLASS()
class BRAWLINN_API AThrowable_B : public AItem_B
{
	GENERATED_BODY()
	
public:

	AThrowable_B();

	UPROPERTY(VisibleAnywhere)
		UDestructibleComponent* DestructibleComponent;


	UFUNCTION()
	void OnComponentFracture(const FVector& HitPoint, const FVector& HitDirection);

	// ** Overridden functions **
		virtual void BeginPlay() override;

	virtual void PickedUp_Implementation(ACharacter_B* Player) override;

	virtual void Dropped_Implementation() override;

	virtual void Use_Implementation() override;
	virtual void OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

protected:
	// ** Variables **

	UPROPERTY(EditAnywhere, Category = "Variables")
	float ThrowHitStrength = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Score", meta = (Tooltip = "This value is overridden if ShouldUseSpreadSheets is enabled"))
	int ScoreAmount = 1;

};
