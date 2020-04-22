// Fill out your copyright notice in the Description page of Project Settings.


#include "ComboBoxString_B.h"

void UComboBoxString_B::Tick_Implementation()
{
	if (IsHovered())
		SetUserFocus(GetOwningPlayer());

	if (HasUserFocus(GetOwningPlayer()))
		
		WidgetStyle.ComboButtonStyle.SetButtonStyle(Selected);

	else
		WidgetStyle.ComboButtonStyle.SetButtonStyle(Unselected);


}
