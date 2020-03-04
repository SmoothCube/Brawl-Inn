#pragma once

#include "CoreMinimal.h"
#include "BarDropLocations.generated.h"

class AAIDropPoint_B;

UENUM()
enum class EBarDropLocationType : uint8
{
	Stool = 0,
	Tankard
};

USTRUCT()
struct FBarDropLocations
{
	GENERATED_BODY()

	FBarDropLocations()
	{

	}
	FBarDropLocations(EBarDropLocationType InType)
	{
		Type = InType;
	}

	void AddBack(AAIDropPoint_B* DropPoint)
	{
		List.Add(DropPoint);
	}

	AAIDropPoint_B* RemoveFront()
	{
		if (List.Num() == 0)
			return nullptr;

		AAIDropPoint_B* DropPoint = List[0];
		List.RemoveAt(0);
		return DropPoint;
	}

	AAIDropPoint_B* PeekFront()
	{
		if (List.Num() == 0)
			return nullptr;
		return List[0];
	}

	EBarDropLocationType Type = EBarDropLocationType::Stool;
private:
	TArray<AAIDropPoint_B*> List;
};