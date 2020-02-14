// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ThrowComponent_B.generated.h"

class ACharacter_B;
class AGameMode_B;
class UHoldComponent_B;

UCLASS()
class BRAWLINN_API UThrowComponent_B : public UActorComponent
{
	GENERATED_BODY()

public:
	UThrowComponent_B(const FObjectInitializer& ObjectInitializer);

	// ********** UActorComponent **********
protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ********** Throw **********
public:
	bool TryThrow();

	void StartThrow();

	void Throw();

	bool IsThrowing() const;
protected:
	bool bIsThrowing = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Throw")
		float MinImpulseSpeed = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Throw")
		float MaxImpulseSpeed = 4000.f;

public:
	float ImpulseSpeed = 0.f;
	// ********** Charging **********

	bool IsCharging() const;
protected:
	bool bIsCharging = false;
public:
	float ImpulseTimer = 0.f;

	// ********** AimAssist **********
	bool AimAssist(FVector& TargetPlayerLocation);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|AimAssist")
		float AimAssistAngle = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|AimAssist")
		float AimAssistRange = 2000.f;

	// ********** Misc. **********

	UFUNCTION()
		void OneCharacterChanged();

	UPROPERTY()
		TArray<ACharacter_B*> OtherPlayers;

	UPROPERTY()
		AGameMode_B* GameMode = nullptr;

	UPROPERTY()
		UHoldComponent_B* HoldComponent = nullptr;

	UPROPERTY()
		ACharacter_B* OwningCharacter = nullptr;

	bool IsReady() const;
};
