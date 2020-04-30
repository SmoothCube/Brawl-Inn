// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterVictoryScreen_B.h"
#include "TextBlock.h"
#include "Image.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetMaterialLibrary.h"

void UCharacterVictoryScreen_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(BackgroundColor->Brush.GetResourceObject());
	if (MaterialInstance)
	{
		BackgroundMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), MaterialInstance);
		BackgroundColor->SetBrushFromMaterial(BackgroundMaterial);
	}

	ScoreNumber->SetText(FText::AsNumber(0));
}
