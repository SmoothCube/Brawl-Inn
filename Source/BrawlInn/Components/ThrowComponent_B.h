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

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;

	bool IsReady() const;
	bool TryThrow();
	void Throw();

	UFUNCTION(BlueprintPure)
	bool IsCharging() const;

	UFUNCTION(BlueprintPure)
	bool IsThrowing() const;

	void StartThrow();

	bool AimAssist(FVector& TargetPlayerLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throw")
		float MinImpulseSpeed = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throw")
		float MaxImpulseSpeed = 4000.f;

	float ImpulseSpeed = 0.f;

	float ImpulseTimer = 0.f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OneCharacterChanged();


	/// Aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimAssist")
		float AimAssistAngle = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimAssist")
		float AimAssistRange = 2000.f;

private:
	bool bIsCharging = false;
	bool bIsThrowing = false;

	TArray<ACharacter_B*> OtherPlayers;

	UPROPERTY()
	AGameMode_B* GameMode = nullptr;
	UPROPERTY()
	UHoldComponent_B* HoldComponent = nullptr;
	UPROPERTY()
	ACharacter_B* OwningCharacter = nullptr;
};
