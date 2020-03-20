// Fill out your copyright notice in the Description page of Project Settings.

#include "Slider_B.h"

#if WITH_EDITOR
void USlider_B::OnCreationFromPalette()
{
	MouseUsesStep = true;
	RequiresControllerLock = false;

	FSlateBrush Brush = WidgetStyle.NormalThumbImage;
	Brush.SetImageSize(FVector2D(16, 28));
	
	WidgetStyle.SetNormalThumbImage(Brush);
	WidgetStyle.SetHoveredThumbImage(Brush);
	WidgetStyle.SetDisabledThumbImage(Brush);
	WidgetStyle.SetBarThickness(10);
	
	Super::OnCreationFromPalette();
}
#endif

void USlider_B::Tick_Implementation()
{
	if (IsHovered())
		SetUserFocus(GetOwningPlayer());
	
	if (HasUserFocus(GetOwningPlayer()))
		SetSliderHandleColor(SelectedHandle);
	else
		SetSliderHandleColor(UnselectedHandle);
}
