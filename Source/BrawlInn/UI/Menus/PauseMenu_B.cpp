// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenu_B.h"
#include "Components/Button.h"

#include "BrawlInn.h"

bool UPauseMenu_B::Initialize()
{
	bool success = Super::Initialize();

	ContinueButton->OnClicked.AddDynamic(this, &UPauseMenu_B::ContinueButtonClicked);
	ExitButton->OnClicked.AddDynamic(this, &UPauseMenu_B::ExitButtonClicked);

	return success;
}

void UPauseMenu_B::ContinueButtonClicked()
{
	BScreen("Playbutton clicked");
}

void UPauseMenu_B::ExitButtonClicked()
{
	BScreen("Playbutton clicked");
}
