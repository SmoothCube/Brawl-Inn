// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "System/Enums/Charge.h"
#include "PunchComponent_B.generated.h"


class ACharacter_B;
class USoundCue;
class UDamageType;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGetPunched, ACharacter_B*);

DECLARE_MULTICAST_DELEGATE(FOnPunchHit);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRAWLINN_API UPunchComponent_B : public USphereComponent
{
	GENERATED_BODY()

public:	
	UPunchComponent_B();
	
	// ********** AActor **********
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	ACharacter_B* OwningCharacter = nullptr;
public:
	// ********** Punch **********
	UFUNCTION(BlueprintCallable)
	void PunchStart();

	void PunchDash();

	UFUNCTION(BlueprintCallable)
	void PunchEnd();
	
	UFUNCTION(BlueprintCallable)
	bool GetIsPunching();

protected:	
	void PunchHit(ACharacter_B* OtherPlayer);
	void PunchHit(UPrimitiveComponent* OtherComp);

	void GetPunched(FVector InPunchStrength, ACharacter_B* PlayerThatPunched);

	//For health
	float CalculatePunchDamage(ACharacter_B* OtherPlayer);
	
	//For knockback
	//UFUNCTION(Meta)
	//calculates the punch strength for the player. Has to be used by the puncher.
	FVector CalculatePunchStrength();

	void SetIsPunching(bool Value);

public:
	bool bIsPunching = false;

	// ********** ChargePunch **********
	void SetChargeLevel(EChargeLevel chargeLevel);

	bool GetIsCharging();
	void SetIsCharging(bool Value);

	UPROPERTY(EditAnywhere, Category = "Variables | Charge")
	float ChargeTier2Percentage = 0.45;

	UPROPERTY(EditAnywhere, Category = "Variables | Charge")
	float ChargeTier3Percentage = 0.9;

protected:
	UPROPERTY(EditAnywhere, Category = "Variables | Charge")
	float Level3PunchStrength = 1000000;		// this is used in fall

	UPROPERTY(EditAnywhere, Category = "Variables | Charge")
	float Level2PunchStrength = 300000;		// this is used in movementComponent
	
	UPROPERTY(EditAnywhere, Category = "Variables | Charge")
	float Level1PunchStrength = 150000.f;	// this is used in movement

	UPROPERTY(EditAnywhere, Category = "Variables | Charge")
	float Charge1MoveSpeed = 500.f;

	UPROPERTY(EditAnywhere, Category = "Variables | Charge")
	float Charge2MoveSpeed = 250.f;

	UPROPERTY(EditAnywhere, Category = "Variables | Charge")
	float Charge3MoveSpeed = 100.f;

private:
	EChargeLevel ChargeLevel;
	bool bIsCharging = false;

protected:
	bool bHasHit = false;

	UPROPERTY(EditAnywhere, Category = "Variables | Punch")
	float PunchHitVelocityDamper = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Variables | Punch")
	float BasePunchStrength = 150000.f;

	UPROPERTY(EditAnywhere, Category = "Variables | Punch")
	float PunchStrengthMultiplier = 135.f;

	UPROPERTY(EditAnywhere, Category = "Variables | Punch")
	float PunchWaitingTime = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Variables | PunchDash")
	float Charge1PunchDashSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Variables | PunchDash")
	float Charge2PunchDashSpeed = 2000.f;

	UPROPERTY(EditAnywhere, Category = "Variables | PunchDash")
	float Charge3PunchDashSpeed = 3500.f;

	UPROPERTY(EditAnywhere, Category = "Variables | Audio")
	USoundCue* ChargePunchSound = nullptr;

	FTimerHandle TH_PunchAgainHandle;

public:
	FOnGetPunched OnGetPunched_D;

	FOnPunchHit OnPunchHit_D;

	// ********** Dash **********
	void Dash();
protected:

	FVector VelocityBeforeDash = FVector::ZeroVector;
protected:

	UPROPERTY(EditAnywhere, Category = "Variables | Dash")
	float DashSpeed = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Variables | Dash")
	float PostDashRemainingVelocityPercentage = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Variables | Dash")
	float DashCooldown = 2.f;

	UPROPERTY(EditAnywhere, Category = "Variables | Dash")
	float DashTime = 0.1f;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables | Dash", meta = (ToolTip = "The percentage of a player's velocity that another character will be pushed with if this player dashes through them"))
	float DashPushPercentage = 0.5f;
	
	bool GetIsDashing();
private:
	bool bIsDashing = false;

	FTimerHandle TH_DashAgainHandle;
	FTimerHandle TH_DashDoneHandle;
protected:
	// ********** Various **********
	UPROPERTY(EditAnywhere, Category = "Variables | Audio")
	USoundCue* PunchSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables | Damage")
	TSubclassOf<UDamageType> BP_DamageType;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
