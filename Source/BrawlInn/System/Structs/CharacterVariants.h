#pragma once

#include "CoreMinimal.h"
#include "CharacterVariants.generated.h"

class UMaterialInterface;
class UPaperSprite;
class UTexture2D;

USTRUCT(BlueprintType)
struct FCharacterVariants
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInterface* MeshMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor TextColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInterface* IndicatorMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* BarrelTexture;

	bool bTaken = false;

};