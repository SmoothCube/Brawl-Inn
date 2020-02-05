// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent_B.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FHealthUpdate, int);
DECLARE_MULTICAST_DELEGATE(FZeroHealth);
DECLARE_MULTICAST_DELEGATE(FRespawn);

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

	FHealthUpdate HealthUpdated_D;

	FZeroHealth HealthIsZero_D;

	FRespawn OnRespawn_D;

	UFUNCTION()
	void SetHealthWidget(UHealthWidget_B* Widget);

	UPROPERTY()
	UHealthWidget_B* HealthWidget = nullptr;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Variables")
		int Respawns = 3;

	UPROPERTY()
		int Health = 100;


private:

	int StartingHealth = 100;
};
