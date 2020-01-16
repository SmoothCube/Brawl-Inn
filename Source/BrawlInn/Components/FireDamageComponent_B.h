// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FireDamageComponent_B.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFireDamageStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFireDamageStop);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FZeroFireHealth);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRAWLINN_API UFireDamageComponent_B : public UActorComponent
{
	GENERATED_BODY()

public:
	UFireDamageComponent_B();

protected:

	// ** Overridden functions **
	virtual void BeginPlay() override;

public:

	// ** Delegates **

	UPROPERTY(BlueprintAssignable, Category = Delegates)
		FFireDamageStart FireDamageStart_D;

	UPROPERTY(BlueprintAssignable, Category = Delegates)
		FFireDamageStop FireDamageStop_D;

	UPROPERTY(BlueprintAssignable, Category = Delegates)
		FZeroFireHealth FireHealthIsZero_D;

	// ** Functions **

	UFUNCTION(BlueprintCallable)
		int GetFireHealth() const;

protected:

	UFUNCTION()
		void TakeDamage();

	UFUNCTION()
		void StartFireDamage();

	UFUNCTION()
		void StopFireDamage();

	UFUNCTION()
		void TakeFireDamage();

	UFUNCTION()
		void ResetFireTimer();

	// ** Variables ** 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variables")
		float FireHealthAmount = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float FireTick = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float FireDamagePerTick = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float FireStopTimer = 5.f;

private:

	FTimerHandle TH_IsOnFire;

	FTimerHandle TH_ResetFire;

	float FireHealthMaxAmount = 100;
};
