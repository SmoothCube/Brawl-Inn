#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ScoreLookupTable.generated.h"

USTRUCT(BlueprintType)
struct FScoreTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
	int Value;
};