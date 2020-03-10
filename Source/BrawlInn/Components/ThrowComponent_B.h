// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ThrowComponent_B.generated.h"

class ACharacter_B;
class AGameMode_B;
class UHoldComponent_B;
class USoundCue;

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

public:

	// ********** AimAssist **********
	bool AimAssist(FVector& TargetPlayerLocation);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|AimAssist")
		float AimAssistAngle = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|AimAssist")
		float AimAssistRange = 1000.f;

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
