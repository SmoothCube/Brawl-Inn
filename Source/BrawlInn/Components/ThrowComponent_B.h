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

	bool IsReady() const;
	bool TryThrow();
	void Throw();

	UFUNCTION(BlueprintPure)
		bool IsThrowing() const;

	bool AimAssist(FVector& TargetPlayerLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throw")
		float ImpulseSpeed = 1500.f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OneCharacterChanged();



	/// Aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimAssist")
		float AimAssistAngle = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimAssist")
		float AimAssistRange = 2000.f;

private:
	bool bIsThrowing = false;

	TArray<ACharacter_B*> OtherPlayers;

	AGameMode_B* GameMode = nullptr;
	UHoldComponent_B* HoldComponent = nullptr;
	ACharacter_B* OwningPlayer = nullptr;
};
