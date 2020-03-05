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

	virtual bool IsHeld_Implementation() const override;

	virtual bool CanBeHeld_Implementation() const override;

	virtual float GetThrowStrength_Implementation(EChargeLevel level) const override;

	UPROPERTY(EditAnywhere, Category = "Variables|Throw")
	float Charge1ThrowStrength = 1000.f;
	
	UPROPERTY(EditAnywhere, Category = "Variables|Throw")
	float Charge2ThrowStrength = 2000.f;
	
	UPROPERTY(EditAnywhere, Category = "Variables|Throw")
	float Charge3ThrowStrength = 3000.f;

	// ********** Destroy/Fracture **********
public:
	FOnFracture& OnFracture();

protected:
	FOnFracture OnFracture_Delegate;

	//Called when OnFracture is broadcasted
	virtual void OnItemFracture();

	bool bIsFractured = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Juice|Destroy")
		UNiagaraSystem* PS_OnDestroy;

	UPROPERTY(EditAnywhere, Category = "Juice|Destroy")
		USoundCue* DestroyedCue;

	// ********** Misc. **********

	UFUNCTION()
		void OnThrowOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		TSubclassOf<UDamageType> BP_DamageType;

	UPROPERTY()
		ACharacter_B* OwningCharacter = nullptr;
};
