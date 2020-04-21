// Fill out your copyright notice in the Description page of Project Settings.

#include "ControllerLayout_B.h"

#include "UI/UIElements/Button_B.h"

void UControllerLayout_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	BackButton->Execute_Tick(BackButton);
}

UButton_B* UControllerLayout_B::GetBackButton() const
{
	return BackButton;
}
