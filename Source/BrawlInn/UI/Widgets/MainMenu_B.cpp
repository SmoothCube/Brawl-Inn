// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu_B.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Font.h"

#include "BrawlInn.h"
#include "ConfigCacheIni.h"
#include "ControllerLayout_B.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/MenuGameMode_B.h"
#include "TextBlock.h"
#include "UI/UIElements//Button_B.h"
#include "UI/Widgets/Settings/SettingsWidget_B.h"

void UMainMenu_B::NativeConstruct()
{
	Super::NativeConstruct();

	GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	check(IsValid(GameMode));
	check(IsValid(GameInstance));

	PlayButton->OnClicked.AddDynamic(GameMode, &AMenuGameMode_B::OnMenuPlayButtonClicked);
	SettingsButton->OnClicked.AddDynamic(this, &UMainMenu_B::SettingsButtonClicked);
	SettingsWidget->GetBackButton()->OnClicked.AddDynamic(this, &UMainMenu_B::BackFromSettingsButtonClicked);
	ControlsButton->OnClicked.AddDynamic(this, &UMainMenu_B::ControlsButtonClicked);
	CreditsButton->OnClicked.AddDynamic(this, &UMainMenu_B::CreditsButtonClicked);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu_B::QuitButtonClicked);

	UIElementsWithInterface.Add(PlayButton);
	UIElementsWithInterface.Add(SettingsButton);
	UIElementsWithInterface.Add(CreditsButton);
	UIElementsWithInterface.Add(ControlsButton);
	UIElementsWithInterface.Add(QuitButton);

	ControllerLayout->SetVisibility(ESlateVisibility::Collapsed);


	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(PlayButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);

}

void UMainMenu_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FSlateFontInfo UnSelectedFontInfo;
	UnSelectedFontInfo.FontObject = TextFont;
	UnSelectedFontInfo.TypefaceFontName = "Light";
	UnSelectedFontInfo.Size = 42;

	const FSlateColor UnSelectedColor(FLinearColor(0.930111f, 0.623960f, 0.147027f, 1));

	FSlateFontInfo SelectedFontInfo;
	SelectedFontInfo.FontObject = TextFont;
	SelectedFontInfo.TypefaceFontName = "Italic";
	SelectedFontInfo.Size = 42;

	const FSlateColor SelectedColor(FLinearColor(1.000000f, 0.896269f, 0.376262f, 1));

	PlayButton->SetTextAndSettings(PlayText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor, true);
	SettingsButton->SetTextAndSettings(SettingsText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor, true);
	ControlsButton->SetTextAndSettings(ControlsText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor, true);
	CreditsButton->SetTextAndSettings(CreditsText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor, true);
	QuitButton->SetTextAndSettings(QuitText, UnSelectedFontInfo, UnSelectedColor, SelectedFontInfo, SelectedColor, true);

	SettingsWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UMainMenu_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	for (UWidget* Element : UIElementsWithInterface)
	{
		if (IsValid(Element) && Element->GetClass()->ImplementsInterface(UUIElementsInterface_B::StaticClass()))
			IUIElementsInterface_B::Execute_Tick(Element);
	}

	if (SettingsButton->HasAnyUserFocus())
	{
		SettingsBlockAnimationForward();
	}
	else
	{
		SettingsBlockAnimationReverse();
	}
}

void UMainMenu_B::SettingsButtonClicked()
{
	bIsInsideSetting = true;
	for (auto Widget : UIElementsWithInterface)
	{
		Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	SettingsWidget->SetVisibility(ESlateVisibility::Visible);

	FOnInputAction FaceButtonRightDelegate; 
	FaceButtonRightDelegate.BindDynamic(this, &UMainMenu_B::BackFromSettingsButtonClicked);
	ListenForInputAction("FaceButtonRight", IE_Pressed, true, FaceButtonRightDelegate);

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(SettingsWidget->GetBackButton()->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);

}

void UMainMenu_B::ControlsButtonClicked()
{
	ControllerLayout->SetVisibility(ESlateVisibility::Visible);

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(ControllerLayout->GetBackButton()->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);

	ControllerLayout->GetBackButton()->OnPressed.AddDynamic(this, &UMainMenu_B::ControllerLayoutBackButtonClicked);

	FOnInputAction FaceButtonRightDelegate; //This doesnt work, maybe because mainmenu isnt in focus anymore?
	FaceButtonRightDelegate.BindDynamic(this, &UMainMenu_B::ControllerLayoutBackButtonClicked);
	ListenForInputAction("FaceButtonRight", IE_Pressed, true, FaceButtonRightDelegate);
}

void UMainMenu_B::ControllerLayoutBackButtonClicked()
{
	ControllerLayout->SetVisibility(ESlateVisibility::Collapsed);

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(ControlsButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);

	ControllerLayout->GetBackButton()->OnPressed.RemoveDynamic(this, &UMainMenu_B::ControllerLayoutBackButtonClicked);

	StopListeningForInputAction("FaceButtonRight", IE_Pressed);
}

void UMainMenu_B::CreditsButtonClicked()
{
	BScreen("Credits");
}

void UMainMenu_B::QuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

void UMainMenu_B::BackFromSettingsButtonClicked()
{
	for (auto Widget : UIElementsWithInterface)
	{
		Widget->SetVisibility(ESlateVisibility::Visible);
	}

	StopListeningForInputAction("FaceButtonBottom", IE_Pressed);

	SettingsWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(SettingsButton->GetCachedWidget());
	GetOwningPlayer()->SetInputMode(InputMode);

	bIsInsideSetting = false;

	check(IsValid(GameInstance));

	GConfig->SetFloat(TEXT("BrawlInn.Audio"), TEXT("Master Volume"), GameInstance->GetMasterVolume(), GGameIni);
	GConfig->SetFloat(TEXT("BrawlInn.Audio"), TEXT("Music Volume"), GameInstance->GetMusicVolume(), GGameIni);
	GConfig->SetFloat(TEXT("BrawlInn.Audio"), TEXT("Sfx Volume"), GameInstance->GetSfxVolume(), GGameIni);
	GConfig->Flush(false, GGameIni);
}
