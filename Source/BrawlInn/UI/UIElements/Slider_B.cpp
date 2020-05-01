// Fill out your copyright notice in the Description page of Project Settings.

#include "Slider_B.h"
#include "TextBlock.h"

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
	if (IsValid(SliderText))
	{
		if (IsHovered())
			SetUserFocus(GetOwningPlayer());

		if (HasUserFocus(GetOwningPlayer()))
		{
			WidgetStyle.SetNormalBarImage(SelectedNormalBarImage);
			WidgetStyle.SetNormalThumbImage(SelectedNormalThumbImage);
			SliderText->SetFont(SelectedFont);
			SliderText->SetColorAndOpacity(SelectedColor);
		}
		else
		{
			WidgetStyle.SetNormalBarImage(UnSelectedNormalBarImage);
			WidgetStyle.SetNormalThumbImage(UnSelectedNormalThumbImage);
			SliderText->SetFont(UnSelectedFont);
			SliderText->SetColorAndOpacity(UnSelectedColor);
		}
	}
}

void USlider_B::SetTextAndSettings(UTextBlock* Text, FSlateFontInfo UnSelectedFontInfo, FSlateColor UnSelectedColorIn,
	FSlateFontInfo SelectedFontInfo, FSlateColor SelectedColorIn)
{
	if (Text)
	{
		SliderText = Text;
		UnSelectedFont = UnSelectedFontInfo;
		UnSelectedColor = UnSelectedColorIn;
		SelectedFont = SelectedFontInfo;
		SelectedColor = SelectedColorIn;
	}
}
