// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ThrowComponent_B.generated.h"

class APlayerCharacter_B;
class AGameMode_B;
class UHoldComponent_B;

UCLASS()
class BRAWLINN_API UThrowComponent_B : public USceneComponent
{
	GENERATED_BODY()
public:
	UThrowComponent_B(const FObjectInitializer& ObjectInitializer);

	bool IsReady() const;
	bool TryThrow();
	void Throw();

	UFUNCTION(BlueprintPure)
		bool IsThrowing() const;

protected:
	virtual void BeginPlay() override;

	bool AimAssist(FVector& TargetPlayerLocation);

	UFUNCTION()
	void OneCharacterChanged();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throw")
		float ImpulseSpeed = 1500.f;

	/// Aiming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimAssist")
		float AimAssistAngle = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimAssist")
		float AimAssistRange = 2000.f;

private:
	bool bIsThrowing = false;

	TArray<APlayerCharacter_B*> OtherPlayers;

	AGameMode_B* GameMode = nullptr;
	UHoldComponent_B* HoldComponent = nullptr;
	APlayerCharacter_B* OwningPlayer = nullptr;
};
