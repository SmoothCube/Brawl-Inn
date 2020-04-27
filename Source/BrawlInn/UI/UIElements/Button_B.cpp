// Fill out your copyright notice in the Description page of Project Settings.

#include "Button_B.h"
#include "TextBlock.h"

void UButton_B::Tick_Implementation()
{
	if (!IsValid(ButtonText))
		return;

	if (bShouldUpdateStyle)
	{
		if (IsHovered())
			SetUserFocus(GetOwningPlayer());

		if (HasUserFocus(GetOwningPlayer()))
		{
			ButtonText->SetFont(SelectedFont);
			if (bShouldIndent)
				ButtonText->SetRenderTranslation(FVector2D(15, 0));
			ButtonText->SetColorAndOpacity(SelectedColor);
			SetRenderScale(FVector2D(1, 1));

		}
		else
		{
			ButtonText->SetFont(UnSelectedFont);
			if (bShouldIndent)
				ButtonText->SetRenderTranslation(FVector2D(0, 0));
			ButtonText->SetColorAndOpacity(UnSelectedColor);
			SetRenderScale(FVector2D(1, 1));
		}
	}
	else
	{
		ButtonText->SetFont(SelectedFont);
		if (bShouldIndent)
			ButtonText->SetRenderTranslation(FVector2D(15, 0));
		ButtonText->SetColorAndOpacity(SelectedColor);
		SetRenderScale(FVector2D(1, 1));
	}
}

void UButton_B::SetTextAndSettings(UTextBlock* Text, FSlateFontInfo UnSelectedFontInfo, FSlateColor UnSelectedColorIn, FSlateFontInfo SelectedFontInfo, FSlateColor SelectedColorIn, bool
	bShouldIndentIn)
{
	if (Text)
	{
		bShouldUpdateStyle = true;
		ButtonText = Text;
		UnSelectedFont = UnSelectedFontInfo;
		UnSelectedColor = UnSelectedColorIn;
		SelectedFont = SelectedFontInfo;
		SelectedColor = SelectedColorIn;
		bShouldIndent = bShouldIndentIn;
	}
}

void UButton_B::ShouldUpdateStyle(const bool bShouldUpdateStyleIn)
{
	bShouldUpdateStyle = bShouldUpdateStyleIn;
}
