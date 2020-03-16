// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Character_B.h"
#include "IdleAICharacter_B.generated.h"

UCLASS()
class BRAWLINN_API AIdleAICharacter_B : public ACharacter_B
{
	GENERATED_BODY()

protected:
	// ********** AActor **********
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void FellOutOfWorld(const UDamageType& dmgType) override;

	// ********** ACharacter_B **********

	void SetState(EState s) override;

	// ********** Location/Movement **********

	UPROPERTY(BlueprintReadOnly, Category = "Variables|Movement")
		FVector StartLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Variables|Movement")
		FVector RespawnLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Variables|Movement")
		bool bCanMove = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variables|Movement")
		float ResetCanMoveTime = 60.f;

	FTimerHandle TH_ResetCanMove;



};
