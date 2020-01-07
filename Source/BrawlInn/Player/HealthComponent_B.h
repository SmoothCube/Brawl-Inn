// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent_B.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthUpdate, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FZeroHealth);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRAWLINN_API UHealthComponent_B : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent_B();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		int GetHealth();

	UFUNCTION(BlueprintCallable)
		void SetHealth(int Value);

	UFUNCTION(BlueprintCallable)
		void TakeDamage(int Value = 1);

	UPROPERTY(BlueprintAssignable, Category = ScoreSubsystem)
		FHealthUpdate HealthUpdated_D;

	UPROPERTY(BlueprintAssignable, Category = ScoreSubsystem)
		FZeroHealth HealthIsZero_D;

protected:
	virtual void BeginPlay() override;

private:

	int Health = 5;
};
