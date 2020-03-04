#pragma once

#include "CoreMinimal.h"
#include "Charge.generated.h"

UENUM(BlueprintType)
enum class EChargeLevel : uint8
{
	ENotCharging	UMETA(DisplayName = "Not Charging"),
	EChargeLevel1 	UMETA(DisplayName = "Charge Level 1"),
	EChargeLevel2	UMETA(DisplayName = "Charge Level 2"),
	EChargeLevel3	UMETA(DisplayName = "Charge Level 3")
};