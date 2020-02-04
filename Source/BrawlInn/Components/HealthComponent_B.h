// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent_B.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthUpdate, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FZeroHealth);

class UHealthWidget_B;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRAWLINN_API UHealthComponent_B : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent_B();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetHealth() const;

	UFUNCTION(BlueprintCallable)
	void SetHealth(int Value);

	UFUNCTION(BlueprintCallable)
	void TakeDamage(int Value = 1);

	UPROPERTY(BlueprintAssignable, Category = Delegates)
	FHealthUpdate HealthUpdated_D;

	UPROPERTY(BlueprintAssignable, Category = Delegates)
	FZeroHealth HealthIsZero_D;

	void SetHealthWidget(UHealthWidget_B* Widget);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variables")
		int Health = 12;

	UPROPERTY()
	UHealthWidget_B* HealthWidget = nullptr;
};
