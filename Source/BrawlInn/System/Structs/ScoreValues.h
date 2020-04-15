#pragma once

#include "CoreMinimal.h"
#include "ScoreValues.generated.h"

UENUM(BlueprintType)
enum EScoreValueTypes
{
	Score = 0,
	ThrowablesHit, PunchesHit, OutOfMapDeaths, CrownTime
};

USTRUCT(BlueprintType)
struct FScoreValues {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		int Score = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int ThrowablesHit = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int PunchesHit = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int CrownTime = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int OutOfMapDeaths = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int LastScoreAdded = 0;
};