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

protected:

	AThrowable_B();

	// ********** AActor **********

	virtual void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ********** Throwing **********

	virtual void PickedUp_Implementation(ACharacter_B* Player) override;

	virtual void Dropped_Implementation() override;

	virtual void Use_Implementation() override;

	virtual void OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(EditAnywhere)
		float ThrowHitStrength = 100;

	// ********** Components **********

public:

	UDestructibleComponent* GetDestructibleComponent() const;
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UDestructibleComponent* DestructibleComponent;

	// ********** Destroy/Fracture **********

	UFUNCTION()
		void OnComponentFracture(const FVector& HitPoint, const FVector& HitDirection); 

	void BeginDespawn();

	void StartDestroy();

	UPROPERTY(EditAnywhere, Category = "Despawning")
		float DownValuePerTick = 10;

	UPROPERTY(EditAnywhere, Category = "Despawning")
		float TimeBeforeDespawn = 5.f;

	FTimerHandle TH_Destroy;
	FTimerHandle TH_Despawn;

	// ********** Score **********
	UPROPERTY(EditDefaultsOnly, Category = "Score", meta = (Tooltip = "This value is overridden if ShouldUseSpreadSheets is enabled"))
		int ScoreAmount = 1;

};
