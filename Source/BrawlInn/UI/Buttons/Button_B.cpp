// Fill out your copyright notice in the Description page of Project Settings.


#include "Button_B.h"
#include "BrawlInn.h"

void UButton_B::Tick_Implementation()
{
	if (IsHovered())
		SetUserFocus(GetOwningPlayer());

	 if (HasUserFocus(GetOwningPlayer()))
		 SetStyle(Selected);
	 else
		 SetStyle(Unselected);
}
