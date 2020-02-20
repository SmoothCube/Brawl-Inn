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
	// Sets default values for this component's properties
	UPunchComponent_B();

	UFUNCTION(BlueprintCallable)
	bool GetIsPunching();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	ACharacter_B* OwningCharacter = nullptr;

public:	

	UFUNCTION(BlueprintCallable)
	void PunchStart();

	void PunchDash();
	void Dash();

	UFUNCTION(BlueprintCallable)
	void PunchEnd();
	
	void PunchHit(ACharacter_B* OtherPlayer);
	void PunchHit(UPrimitiveComponent* OtherComp);

	void GetPunched(FVector InPunchStrength, ACharacter_B* PlayerThatPunched);

	float CalculatePunchDamage(ACharacter_B* OtherPlayer);

	FVector CalculatePunchStrength();

	FOnGetPunched OnGetPunched_D;

	FOnPunchHit OnPunchHit_D;

	UPROPERTY(BlueprintReadWrite, Editanywhere)
	bool bIsPunching = false;

	FVector VelocityBeforeDash = FVector::ZeroVector;
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
	
	FTimerHandle TH_PunchAgainHandle;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float MinPunchDashDistance = 150.f;
	
	UPROPERTY(EditAnywhere, Category = "Dash")
	float MaxPunchDashDistance = 300.f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashForceModifier = 75000.f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashDistance = 22500000.f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float PostDashRemainingVelocityPercentage = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* PunchSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UDamageType> BP_DamageType;
};
