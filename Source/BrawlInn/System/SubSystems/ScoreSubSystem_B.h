// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "System/Structs/ScoreValues.h"
#include "ScoreSubSystem_B.generated.h"

DECLARE_EVENT_OneParam(UScoreSubSystem_B, FOnScoreValuesChanged, FScoreValues);

UCLASS()
class BRAWLINN_API UScoreSubSystem_B : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
private:
	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

public:
	void AddScore(float Score);
	
	void ResetScoreValues();
	
	FScoreValues GetScoreValues() const;

	FOnScoreValuesChanged OnScoreValuesChanged;
private:

	FScoreValues ScoreValues;
};
