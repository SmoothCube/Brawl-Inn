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

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

	void SetIsPunching(bool Value);

	UFUNCTION(BlueprintCallable)
	bool GetCanPunch();

	void SetCanPunch(bool Value);

protected:
	void PunchHit(ACharacter_B* OtherPlayer);
	void PunchHit(UPrimitiveComponent* OtherComp);

	void GetPunched(FVector InPunchStrength, ACharacter_B* PlayerThatPunched);

	//For health
	int CalculatePunchDamage() const;
	
	//For knockback
	//UFUNCTION(Meta)
	//calculates the punch strength for the player. Has to be used by the puncher.
	FVector CalculatePunchStrength();

	bool bIsPunching = false;

	bool bCanPunch = true;

	// ********** ChargePunch **********

	UPROPERTY(EditAnywhere, Category = "Charge", meta = (ToolTip = "The strength a character is pushed with from a level 1 punch"))
		float Level1PunchPushStrength = 200000.f;												 
	UPROPERTY(EditAnywhere, Category = "Charge", meta = (ToolTip = "The strength a character is pushed with from a level 2 punch"))
		float Level2PunchPushStrength = 500000.f;	

	UPROPERTY(EditAnywhere, Category = "Charge", meta = (ToolTip = "The strength a player's mesh is pushed with when falling from a level 1 punch"))
	float Level1PunchStrength = 500000.f;
	
	UPROPERTY(EditAnywhere, Category = "Charge", meta = (ToolTip = "The strength a player's mesh is pushed with when falling from a level 2 punch"))
	float Level2PunchStrength = 1000000.f;		
	
	UPROPERTY(EditAnywhere, Category = "Charge", meta = (ToolTip = "The strength a player's mesh is pushed with when falling from a level 3 punch"))
	float Level3PunchStrength = 2000000.f;		

	bool bHasHit = false;

	UPROPERTY(EditAnywhere, Category = "Punch")
	float PunchHitVelocityDamper = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Punch")
	float PunchWaitingTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Punch|Dash")
	float Charge1PunchDashSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Punch|Dash")
	float Charge2PunchDashSpeed = 2000.f;

	UPROPERTY(EditAnywhere, Category = "Punch|Dash")
	float Charge3PunchDashSpeed = 3500.f;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* ChargePunchSound = nullptr;

	FTimerHandle TH_PunchAgainHandle;

public:
	FOnGetPunched OnGetPunched_D;

	FOnPunchHit OnHitPlayerPunch_D;

	// ********** Dash **********
	UFUNCTION(BlueprintCallable)
	void Dash();
protected:

	FVector VelocityBeforeDash = FVector::ZeroVector;
protected:

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashSpeed = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float PostDashRemainingVelocityPercentage = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashCooldown = 1.2f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashTime = 0.1f;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta = (ToolTip = "The percentage of a player's velocity that another character will be pushed with if this player dashes through them"))
	float DashPushStrength = 100000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta = (ToolTip = "The angle this player pushes others upwards when  dashing through them"))
	float DashPushUpwardsAngle = 15.f;

	bool GetCanDash();

	void SetCanDash(bool Value);

	bool GetIsDashing();
private:
	bool bIsDashing = false;
	bool bCanDash = true;

	FTimerHandle TH_DashAgainHandle;
	FTimerHandle TH_DashDoneHandle;

	// ********** Score **********

	int Level1ScoreValue = 10;
	int Level2ScoreValue = 20;
	int Level3ScoreValue = 30;
protected:
	// ********** Various **********
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* PunchSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UDamageType> BP_DamageType;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
