// Fill out your copyright notice in the Description page of Project Settings.

#include "VideoSettingsWidget_B.h"

#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"

#include "UI/UIElements/Button_B.h"
#include "UI/UIElements/ComboBoxString_B.h"

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

	FSlateFontInfo UnSelectedFontInfo;
	UnSelectedFontInfo.FontObject = AbadiMTFont;
	UnSelectedFontInfo.TypefaceFontName = "Light";
	UnSelectedFontInfo.Size = 36;

	const FSlateColor UnSelectedColor(FLinearColor(0.930111f, 0.623960f, 0.147027f, 1));

	FSlateFontInfo SelectedFontInfo;
	SelectedFontInfo.FontObject = AbadiMTFont;
	SelectedFontInfo.TypefaceFontName = "Italic";
	SelectedFontInfo.Size = 36;

	const FSlateColor SelectedColor(FLinearColor(1.000000f, 0.896269f, 0.376262f, 1));

	ApplyButton->SetTextAndSettings(ApplyText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor, false);

	UnSelectedFontInfo.TypefaceFontName = "Default";
	UnSelectedFontInfo.Size = 32;

	SelectedFontInfo.TypefaceFontName = "Italic";
	SelectedFontInfo.Size = 32;
	
	FullscreenModeBox->SetTextAndSettings(WindowText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor);
	ScreenResolutionBox->SetTextAndSettings(ResolutionText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor);

	UIElementsWithInterface.Add(ApplyButton);
	UIElementsWithInterface.Add(FullscreenModeBox);
	UIElementsWithInterface.Add(ScreenResolutionBox);
}

void UVideoSettingsWidget_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	for (UWidget* Element : UIElementsWithInterface)
	{
		if (IsValid(Element) && Element->GetClass()->ImplementsInterface(UUIElementsInterface_B::StaticClass()))
			IUIElementsInterface_B::Execute_Tick(Element);
	}
}

UButton_B* UVideoSettingsWidget_B::GetApplyButton() const
{
	return ApplyButton;
}

UComboBoxString_B* UVideoSettingsWidget_B::GetScreenResolutionBox() const
{
	return  ScreenResolutionBox;
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