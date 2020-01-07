// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthWidget_B.h"

bool UHealthWidget_B::Initialize()
{
	bool s = Super::Initialize();

	/*Material = UMaterialInstanceDynamic::Create(Cast<UMaterialInterface>(Image), this);


	FSlateBrush brush;
	brush.SetResourceObject(Material);
	brush.DrawAs = ESlateBrushDrawType::Image;
	brush.SetImageSize(FVector2D(100, 100));
	HealthImage->Brush = brush;*/

	return s;
}