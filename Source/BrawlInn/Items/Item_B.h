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
	
public:	
	AItem_B();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UCapsuleComponent* PickupCapsule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		TSubclassOf<UDamageType> BP_DamageType;

	FOnFracture OnFracture;

protected:
	// ** Overridden functions ** 
	virtual void BeginPlay() override;

	virtual void OnItemFracture();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual bool IsHeld_Implementation() const override;

	virtual void Use_Implementation() override;
	
	// ** Delegates ** 

	UFUNCTION()
		void OnThrowOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// ** Variables **
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Juice")
		UNiagaraSystem* PS_OnDestroy;

	ACharacter_B* OwningCharacter = nullptr;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* DestroyedCue;

};
