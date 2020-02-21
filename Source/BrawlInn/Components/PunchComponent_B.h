// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
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

	// ********** Punch **********
public:	
	UFUNCTION(BlueprintCallable)
	void PunchStart();

	void PunchDash();

	UFUNCTION(BlueprintCallable)
	void PunchEnd();
	
	UFUNCTION(BlueprintCallable)
	bool GetIsPunching();

	void PunchHit(ACharacter_B* OtherPlayer);
	void PunchHit(UPrimitiveComponent* OtherComp);

	void GetPunched(FVector InPunchStrength, ACharacter_B* PlayerThatPunched);

	//For health
	float CalculatePunchDamage(ACharacter_B* OtherPlayer);
	
	//For knockback
	//UFUNCTION(Meta)
	//calculates the punch strength for the player. Has to be used by the puncher.
	FVector CalculatePunchStrength();

	UPROPERTY(BlueprintReadWrite, Editanywhere)
	bool bIsPunching = false;

protected:
	bool bHasHit = false;

	float PunchHitVelocityDamper = 0.3f;

	float MinPunchStrengthToFall = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Punch")
		float BasePunchStrength = 150000.f;

	UPROPERTY(EditAnywhere, Category = "Punch")
		float PunchStrengthMultiplier = 135.f;

	UPROPERTY(EditAnywhere, Category = "Punch")
		float PunchWaitingTime = 0.1f;

	UPROPERTY(EditAnywhere, Category = "PunchDash")
		float MinPunchDashDistance = 150.f;

	UPROPERTY(EditAnywhere, Category = "PunchDash")
		float MaxPunchDashDistance = 300.f;

	UPROPERTY(EditAnywhere, Category = "PunchDash")
		float DashForceModifier = 75000.f;

	FTimerHandle TH_PunchAgainHandle;

public:
	FOnGetPunched OnGetPunched_D;

	FOnPunchHit OnPunchHit_D;

	// ********** Dash **********
	void Dash();
protected:

	FVector VelocityBeforeDash = FVector::ZeroVector;
protected:

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashDistance = 22500000.f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float PostDashRemainingVelocityPercentage = 0.3f;

	// ********** Various **********
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* PunchSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UDamageType> BP_DamageType;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
