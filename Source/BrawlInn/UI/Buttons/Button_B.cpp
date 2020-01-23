// Fill out your copyright notice in the Description page of Project Settings.


#include "Button_B.h"

void UButton_B::ButtonTick()
{

	if (IsHovered())
	{
		SetKeyboardFocus();
		SetStyle(MouseHover);
	}
	else
	{
		if (HasKeyboardFocus())
		{
			SetStyle(Selected);
		}
		else {
			SetStyle(Unselected);
		}
	}
}