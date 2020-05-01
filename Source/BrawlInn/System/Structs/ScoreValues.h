#pragma once

#include "CoreMinimal.h"
#include "ScoreValues.generated.h"

UENUM(BlueprintType)
enum EScoreValueTypes
{
	Score = 0,
	ThrowablesHit,
	PunchesHit,
	OutOfMapDeaths,
	CrownTime,
	BarrelsHit
};

USTRUCT(BlueprintType)
struct FScoreValues {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int Score = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ToolTip = "Indicates if the last score added is multiplied or not"))
		bool bIsMultiplied = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ToolTip = "Indicates if the last score added is against the leader or not"))
		bool bIsAgainstLeader = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int LastScoreAdded = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int ThrowablesHit = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int PunchesHit = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int CrownTime = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int OutOfMapDeaths = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int BarrelsHit = 0;
};