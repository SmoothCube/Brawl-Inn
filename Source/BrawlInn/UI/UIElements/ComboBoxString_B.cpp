// Fill out your copyright notice in the Description page of Project Settings.

#include "ComboBoxString_B.h"
#include "TextBlock.h"

void UComboBoxString_B::Tick_Implementation()
{
	if (IsValid(BoxText))
	{

		if (IsHovered())
			SetUserFocus(GetOwningPlayer());

		if (HasUserFocus(GetOwningPlayer()))
		{
			BoxText->SetFont(SelectedFont);
			BoxText->SetColorAndOpacity(SelectedColor);
		}
		else
		{
			BoxText->SetFont(UnSelectedFont);
			BoxText->SetColorAndOpacity(UnSelectedColor);
		}
	}
}

void UComboBoxString_B::SetTextAndSettings(UTextBlock* Text, FSlateFontInfo UnSelectedFontInfo,
	FSlateColor UnSelectedColorIn, FSlateFontInfo SelectedFontInfo, FSlateColor SelectedColorIn)
{
	if (Text)
	{
		BoxText = Text;
		UnSelectedFont = UnSelectedFontInfo;
		UnSelectedColor = UnSelectedColorIn;
		SelectedFont = SelectedFontInfo;
		SelectedColor = SelectedColorIn;
	}
}