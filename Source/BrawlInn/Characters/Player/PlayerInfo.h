#pragma once

#include "CoreMinimal.h"
#include "System/Structs/CharacterVariants.h"
#include "PlayerInfo.generated.h"

UENUM(BlueprintType)
enum class EPlayerCharacterType : uint8
{
	YUGGO = 0,
	SWIFTBOOT,
	DAGGER,
	LEADJAW
};

USTRUCT(BlueprintType)
struct BRAWLINN_API FPlayerInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EPlayerCharacterType Type = EPlayerCharacterType::YUGGO;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DeprecatedProperty, DeprecationMessage = "Use CharacterVariants instead."))
		FLinearColor PlayerColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterVariants CharacterVariants;

};