// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item_B.h"
#include "Useable_B.generated.h"

class USoundCue;
class UNiagaraComponent;

UCLASS()
class BRAWLINN_API AUseable_B : public AItem_B
{
	GENERATED_BODY()

public:

	AUseable_B();

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* DrinkMesh;

	UPROPERTY(VisibleAnywhere)
		UNiagaraComponent* NiagaraSystemComponent;

	// ** Overridden functions **

	virtual void PickedUp_Implementation(APlayerCharacter_B* Player) override;

	virtual void Dropped_Implementation() override;

	virtual void Use_Implementation() override;

protected:
	virtual void FellOutOfWorld(const UDamageType& dmgType) override;

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Juice")
		USoundCue* DrinkSound;

	UPROPERTY(EditDefaultsOnly, Category = "Variables|Boost", meta = (ToolTip = "The duration of the powerup. Set to -1 if it shouldn't expire by time"))
		float Duration = 5;
	
	FTimerHandle TH_DrinkHandle;

	UFUNCTION()
		virtual void ResetBoost();
};
