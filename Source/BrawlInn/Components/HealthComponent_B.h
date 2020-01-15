// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent_B.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthUpdate, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FZeroHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFireDamageStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFireDamageStop);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FZeroFireHealth);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRAWLINN_API UHealthComponent_B : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent_B();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		int GetHealth() const;

	UFUNCTION(BlueprintCallable)
		int GetFireHealth() const;
	
	UFUNCTION(BlueprintCallable)
		void SetHealth(int Value);

	UFUNCTION(BlueprintCallable)
		void TakeDamage(int Value = 1);

	UFUNCTION()
	void TakeOneDamage();

	UPROPERTY(BlueprintAssignable, Category = Delegates)
		FHealthUpdate HealthUpdated_D;

	UPROPERTY(BlueprintAssignable, Category = Delegates)
		FZeroHealth HealthIsZero_D;

	UPROPERTY(BlueprintAssignable, Category = Delegates)
		FFireDamageStart FireDamageStart_D;

	UPROPERTY(BlueprintAssignable, Category = Delegates)
		FFireDamageStop FireDamageStop_D;

	UPROPERTY(BlueprintAssignable, Category = Delegates)
		FZeroFireHealth FireHealthIsZero_D;


protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void StartFireDamage();
	
	UFUNCTION()
	void StopFireDamage();

	
	FTimerHandle TH_IsOnFire;
	FTimerHandle TH_ResetFire;


	UFUNCTION()
	void ResetFireTimer();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variables")
		int Health = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variables")
		float FireHealthAmount = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float FireTick = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float FireDamagePerTick = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float FireStopTimer = 5.f;

	UFUNCTION()
	void TakeFireDamage();


private:

	float FireHealthMaxAmount = 100;

};
