// Fill out your copyright notice in the Description page of Project Settings.


#include "StatEntry_B.h"

#include "TextBlock.h"

void UStatEntry_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Number->SetText(FText::AsNumber(0));
}
