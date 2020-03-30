// Fill out your copyright notice in the Description page of Project Settings.

#include "Button_B.h"

void UButton_B::Tick_Implementation()
{
	if (bShouldUpdateStyle)
	{
		if (IsHovered())
			SetUserFocus(GetOwningPlayer());

		if (HasUserFocus(GetOwningPlayer()))
			SetStyle(Selected);
		else
			SetStyle(Unselected);
	}
	else
	{
		SetStyle(Pressed);
	}
}

void UButton_B::ShouldUpdateStyle(const bool bShouldUpdateStyleIn)
{
	bShouldUpdateStyle = bShouldUpdateStyleIn;
}
