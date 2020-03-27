// Fill out your copyright notice in the Description page of Project Settings.


#include "VideoSettingsWidget_B.h"

#include "BrawlInn.h"
#include "Components/ComboBoxString.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/Buttons/Button_B.h"

void UVideoSettingsWidget_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//Screen Resolution

	TArray<FIntPoint> ScreenResolutionsSupported;
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(ScreenResolutionsSupported);


	for (FIntPoint& Resolution : ScreenResolutionsSupported)
	{
		const FString ResolutionAsString = FString::FromInt(Resolution.X) + "x" + FString::FromInt(Resolution.Y);
		ScreenResolutionBox->AddOption(ResolutionAsString);
		ScreenResolutions.Add(ResolutionAsString, Resolution);
	}

	const FIntPoint Resolution = GEngine->GetGameUserSettings()->GetScreenResolution();
	const FString ResolutionAsString = FString::FromInt(Resolution.X) + "x" + FString::FromInt(Resolution.Y);
	if (ScreenResolutions.Contains(ResolutionAsString))
	{
		ScreenResolutionBox->SetSelectedOption(ResolutionAsString);
	}

	ScreenResolutionBox->OnSelectionChanged.AddDynamic(this, &UVideoSettingsWidget_B::OnScreenResolutionBoxSelectionChanged);

	// Fullscreen Mode

	FullscreenModeBox->AddOption("Fullscreen");
	FullscreenModeBox->AddOption("Borderless");
	FullscreenModeBox->AddOption("Windowed");

	const EWindowMode::Type FullscreenMode = GEngine->GetGameUserSettings()->GetFullscreenMode();
	switch (FullscreenMode)
	{
	case EWindowMode::Fullscreen:
		FullscreenModeBox->SetSelectedIndex(0);
		break;
	case EWindowMode::WindowedFullscreen:
		FullscreenModeBox->SetSelectedIndex(1);
		break;
	case EWindowMode::Windowed:
		FullscreenModeBox->SetSelectedIndex(2);
		break;
	default:;
	}

	FullscreenModeBox->OnSelectionChanged.AddDynamic(this, &UVideoSettingsWidget_B::OnFullscreenModeSelectionChanged);

	// Apply button

	ApplyButton->OnClicked.AddDynamic(this, &UVideoSettingsWidget_B::OnApplyButtonClicked);
	
}

void UVideoSettingsWidget_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	ApplyButton->Execute_Tick(ApplyButton);
	BackButton->Execute_Tick(BackButton);
}

void UVideoSettingsWidget_B::OnScreenResolutionBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	FString LeftString;
	FString RightString;
	SelectedItem.Split("x", &LeftString, &RightString);
	GEngine->GetGameUserSettings()->SetScreenResolution(FIntPoint(FCString::Atoi(*LeftString),FCString::Atoi(*RightString)));
}

void UVideoSettingsWidget_B::OnFullscreenModeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectedItem == "Fullscreen")
		GEngine->GetGameUserSettings()->SetFullscreenMode(EWindowMode::Fullscreen);
	
	else if(SelectedItem == "Borderless")
		GEngine->GetGameUserSettings()->SetFullscreenMode(EWindowMode::WindowedFullscreen);
	
	else if (SelectedItem == "Windowed")
		GEngine->GetGameUserSettings()->SetFullscreenMode(EWindowMode::Windowed);
}

void UVideoSettingsWidget_B::OnApplyButtonClicked()
{
	GEngine->GetGameUserSettings()->ApplySettings(false);
}