// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "AOEDamageArea_B.generated.h"

DECLARE_DELEGATE_OneParam(FFireActivated, AAOEDamageArea_B*)
DECLARE_DELEGATE_OneParam(FFireDeactivated, AAOEDamageArea_B*)

UCLASS()
class BRAWLINN_API AAOEDamageArea_B : public ATriggerBox
{
	GENERATED_BODY()

public:
	AAOEDamageArea_B();

	void ActivateFire();
	void DeactivateFire();

protected:

	/// ** Overridden functions **


	virtual void Tick(float DeltaTime) override;

	/// ** Delegates **
	FFireActivated FireActivated_D;
	FFireDeactivated FireDeactivated_D;

	/// ** Variables **
	UPROPERTY(EditAnywhere, Category = "Variables")
		TSubclassOf<UDamageType> AOEDamageType;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float MinActiveTime = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float MaxActiveTime = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float MinInActiveTime = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float MaxInActiveTime = 5.f;

	FTimerHandle TH_Timer;
};
