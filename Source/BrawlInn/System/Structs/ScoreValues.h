#pragma once

#include "CoreMinimal.h"
#include "ScoreValues.generated.h"

USTRUCT(BlueprintType)
struct FScoreValues {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		int Score = 0.f;
};