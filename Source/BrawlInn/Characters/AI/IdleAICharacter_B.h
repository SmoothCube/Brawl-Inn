// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Character_B.h"
#include "IdleAICharacter_B.generated.h"

class AAIDropPoint_B;
class UBar_B;
class UMergeMeshComponent_B;
UCLASS()
class BRAWLINN_API AIdleAICharacter_B : public ACharacter_B
{
	GENERATED_BODY()

		AIdleAICharacter_B();
protected:
	// ********** AActor **********
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void FellOutOfWorld(const UDamageType& dmgType) override;

	// ********** Components **********
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UMergeMeshComponent_B* MergeMeshComponent;

	// ********** ACharacter_B **********

	void Die() override;

	void SetState(EState StateIn) override;

	// ********** Applause **********
public:
	UFUNCTION(BlueprintImplementableEvent)
	void PlayApplauseMontage();

protected:
	// ********** Location/Movement **********

	void Respawn();

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		FVector StartLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
		FVector RespawnLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		float ResetCanMoveTime = 5.f;

	FTimerHandle TH_Respawn;

	// ********** Drink ordering**********
public:
	void OrderDrink();

	bool CanOrderDrink() const;

	void TryPickup();
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Ordering")
		TSubclassOf<AAIDropPoint_B> BP_DropPoint;

	UPROPERTY(EditAnywhere, Category = "Ordering")
		bool bCanOrderDrink = true;

	UPROPERTY()
		UBar_B* Bar = nullptr;


};
