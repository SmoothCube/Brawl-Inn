// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item_B.h"
#include "Useable_B.generated.h"

class USoundCue;

UCLASS()
class BRAWLINN_API AUseable_B : public AItem_B
{
	GENERATED_BODY()

public:

	AUseable_B();

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* DrinkMesh;
	
	// ** Overridden functions **

	virtual void PickedUp_Implementation(APlayerCharacter_B* Player) override;

	virtual void Dropped_Implementation() override;

	virtual void Use_Implementation() override;

	virtual void OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

protected:

	UPROPERTY(EditAnywhere)
		USoundCue* DrinkSound;
};
