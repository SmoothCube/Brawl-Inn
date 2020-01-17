// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameMode_B.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "Player/PlayerController_B.h"
#include "Camera/CameraActor.h"
#include "Blueprint/UserWidget.h"
#include "UI/Menus/MainMenu_B.h"

void AMenuGameMode_B::BeginPlay()
{
	Super::BeginPlay();

	SpawnCharacter_D.AddDynamic(this, &AMenuGameMode_B::UpdateViewTarget);
	DespawnCharacter_D.AddDynamic(this, &AMenuGameMode_B::UpdateViewTarget);

}

void AMenuGameMode_B::UpdateViewTarget(APlayerController_B* PlayerController)
{
	if (IsValid(Camera))
		PlayerController->SetViewTargetWithBlend(Camera);
}

void AMenuGameMode_B::UpdateViewTargets()
{
	for (const auto& PlayerController : PlayerControllers)
	{
		if (IsValid(Camera))
			PlayerController->SetViewTargetWithBlend(Camera);
	}

}

void AMenuGameMode_B::ShowMainMenu()
{
	if (!PlayerControllers.IsValidIndex(0))
		return;


	auto Widget = CreateWidget<UMainMenu_B>(PlayerControllers[0], BP_MainMenu);
	Widget->AddToViewport();

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(Widget->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerControllers[0]->SetInputMode(InputModeData);

	PlayerControllers[0]->bShowMouseCursor = true;
}
