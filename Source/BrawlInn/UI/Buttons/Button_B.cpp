// Fill out your copyright notice in the Description page of Project Settings.


#include "Button_B.h"
#include "BrawlInn.h"

void UButton_B::ButtonTick()
{
	if (IsHovered())
	{
		SetUserFocus(GetOwningPlayer());
		SetStyle(MouseHover);
	}
	else
	{
		if (HasUserFocus(GetOwningPlayer()))
		{
			SetStyle(Selected);
		}
		else {
			SetStyle(Unselected);
		}
	}
}