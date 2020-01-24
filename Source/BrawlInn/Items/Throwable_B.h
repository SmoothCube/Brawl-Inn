// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "System/Interfaces/ThrowableInterface_B.h"
#include "Throwable_B.generated.h"

class UCapsuleComponent;
class UNiagaraSystem;
class UDamageType;
class APlayerCharacter_B;

UCLASS()
class BRAWLINN_API AThrowable_B : public AActor, public IThrowableInterface_B
{
	GENERATED_BODY()
	
public:	
	AThrowable_B();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* PickupCapsule;

	virtual void PickedUp_Implementation(APlayerCharacter_B* Player) override;

	virtual void Dropped_Implementation() override;

	virtual bool IsHeld_Implementation() const override;


protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	void OnThrowOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		UNiagaraSystem* ParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		TSubclassOf<UDamageType> BP_DamageType;

	UPROPERTY(EditAnywhere, Category = "Variables")
	float ThrowHitStrength = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Variables")
	int DamageAmount = 1;
private:

	APlayerCharacter_B* OwningPlayer = nullptr;
};
