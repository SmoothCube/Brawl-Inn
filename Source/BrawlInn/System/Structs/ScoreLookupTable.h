#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ScoreLookupTable.generated.h"

USTRUCT(BlueprintType)
struct FScoreLookupTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
		int32 Player;
};