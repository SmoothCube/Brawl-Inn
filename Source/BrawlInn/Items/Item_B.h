// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "System/Interfaces/ThrowableInterface_B.h"
#include "Item_B.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnFracture);

class UCapsuleComponent;
class UNiagaraSystem;
class UDamageType;
class ACharacter_B;
class USoundCue;

UCLASS(DontCollapseCategories)
class BRAWLINN_API AItem_B : public AActor, public IThrowableInterface_B
{
	GENERATED_BODY()
protected:

	AItem_B();

	// ********** AActor **********
	virtual void BeginPlay() override;

	virtual void FellOutOfWorld(const UDamageType& DmgType) override;

	// ********** Components **********
public:
	UStaticMeshComponent* GetMesh() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UCapsuleComponent* PickupCapsule;

	// ********** Throwing **********
public:

	const FVector GetHoldLocation_Implementation() override;

	const FRotator GetHoldRotation_Implementation() override;

protected:

	virtual bool IsHeld_Implementation() const override;

	virtual bool CanBeHeld_Implementation() const override;

	virtual float GetThrowStrength_Implementation(EChargeLevel level) const override;

	virtual float GetPickupWeight_Implementation() const override;

public:
	float GetMovementSpeedWhenHeld_Implementation() const override;

protected:

	UPROPERTY(EditAnywhere, Category = "Throw")
		float MovementSpeedWhenHeld = 600.f;

	UPROPERTY(EditAnywhere, Category = "Throw")
		float Charge1ThrowStrength = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Throw")
		float Charge2ThrowStrength = 2000.f;

	UPROPERTY(EditAnywhere, Category = "Throw")
		float Charge3ThrowStrength = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Throw")
		FRotator HoldRotation = FRotator(0, 0, 0);

	UPROPERTY(EditDefaultsOnly, Category = "Throw")
		FVector HoldLocation = FVector(92.291679f, -63.935005f, 0.000139f);

	UPROPERTY(EditAnywhere, Category = "Throw", meta = (Tooltip = "Used to prioritize what item gets picked up when more than one is avaliable. Higher values will be chosen."))
		float PickupWeight = 1.f;


	// ********** Destroy/Fracture **********
public:
	FOnFracture& OnFracture();

protected:
	FOnFracture OnFracture_Delegate;

	//Called when OnFracture is broadcasted
	virtual void OnItemFracture();

	bool bIsFractured = false;

	bool bIsThrown = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Juice|Destroy")
		UNiagaraSystem* PS_OnDestroy;

	UPROPERTY(EditAnywhere, Category = "Juice|Destroy")
		USoundCue* DestroyedCue;

	// ********** Misc. **********

	UFUNCTION()
		void OnThrowOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		TSubclassOf<UDamageType> BP_DamageType;

	UPROPERTY()
		ACharacter_B* OwningCharacter = nullptr;
};
