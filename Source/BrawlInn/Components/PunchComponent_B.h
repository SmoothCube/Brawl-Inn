// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "PunchComponent_B.generated.h"

class APlayerCharacter_B;
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

	APlayerCharacter_B* Player = nullptr;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void PunchStart();

	void Dash();

	UFUNCTION(BlueprintCallable)
	void PunchEnd();
	
	void PunchHit(APlayerCharacter_B* OtherPlayer);
	void PunchHit(UPrimitiveComponent* OtherComp);

	void GetPunched(FVector InPunchStrength);

	bool bIsPunching = false;

protected:
	bool bHasHit = false;

	float PunchHitVelocityDamper = 0.3f;

	float MinPunchStrengthToFall = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Variables")
	float BasePunchStrength = 175.f;
	
	UPROPERTY(EditAnywhere, Category = "Variables")
	float PunchWaitingTime = 0.1f;

	FTimerHandle TH_PunchAgainHandle;

	void setIsPunchingFalse();

	UPROPERTY(EditAnywhere, Category = "Variables")
	float MinDashDistance = 100.f;
	
	UPROPERTY(EditAnywhere, Category = "Variables")
	float MaxDashDistance = 200.f;

	UPROPERTY(EditAnywhere, Category = "Variables")
	float DashForceModifier = 75000.f;
private:


	bool bIsDashing = false;

};
