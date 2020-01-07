// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter_B.generated.h"

class USphereComponent;
class UPunchComponent_B;

UCLASS()
class BRAWLINN_API APlayerCharacter_B : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter_B();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USphereComponent* PunchSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPunchComponent_B* PunchComponent = nullptr;

	UPROPERTY(EditAnywhere)
	float RecoveryTime = 2.0;

	UPROPERTY(EditAnywhere, Category = "Variables")
	float TimeBeforeFall = 1.f;

	UFUNCTION(BlueprintCallable)
	FRotator GetPrevRotation();



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PunchButtonPressed();
	
	void HandleRotation();

	void HandleMovement(float DeltaTime);

	void Fall();
	void StandUp();

	FVector InputVector = FVector::ZeroVector;
	FVector RotationVector = FVector::ZeroVector;
	FVector PrevRotationVector = FVector::ZeroVector;

private:
	bool bHasFallen = false;
	FTransform RelativeMeshTransform;
	FTimerHandle TH_RecoverTimer;

	float CurrentFallTime = 0.f;

};
