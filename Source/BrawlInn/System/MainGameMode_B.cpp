// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "BrawlInn.h"

#include "System/Camera/GameCamera_B.h"
#include "Player/PlayerController_B.h"
#include "System/GameInstance_B.h"
#include "UI/Menus/PauseMenu_B.h"
#include "UI/Game/VictoryScreenWidget_B.h"

void AMainGameMode_B::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);

	/// Spawns and setups camera
	GameCamera = GetWorld()->SpawnActor<AGameCamera_B>(BP_GameCamera, FTransform());

	/// Spawns characters for the players
	for (int ID : Cast<UGameInstance_B>(GetGameInstance())->ActivePlayerControllerIDs)
	{
		APlayerController_B* PlayerController = Cast<APlayerController_B>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), ID));
		if (!PlayerController) { BError("PlayerController for id %i not found. Check IDs in GameInstance", ID); continue; }

		SpawnCharacter_D.Broadcast(PlayerController,false,FTransform());

	}

	//Create and setup victory widget

	UVictoryScreenWidget_B* VictoryScreen = CreateWidget<UVictoryScreenWidget_B>(PlayerControllers[0], BP_VictoryScreen);
	VictoryScreen->AddToViewport();
	OnPlayerWin.BindUObject(VictoryScreen, &UVictoryScreenWidget_B::OnPlayerWin);

}

void AMainGameMode_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(PauseMenuWidget))
		return;

	PauseMenuWidget->MenuTick();
}

void AMainGameMode_B::UpdateViewTarget(APlayerController_B* PlayerController)
{
	if (IsValid(GameCamera))
		PlayerController->SetViewTargetWithBlend(GameCamera);
}

void AMainGameMode_B::PauseGame(APlayerController_B* ControllerThatPaused)
{
	PlayerControllerThatPaused = ControllerThatPaused;
	PauseMenuWidget = CreateWidget<UPauseMenu_B>(ControllerThatPaused, BP_PauseMenu);
	PauseMenuWidget->AddToViewport();
	FInputModeUIOnly InputMode;
	ControllerThatPaused->SetInputMode(InputMode);
	SetActorTickEnabled(true);
	SetTickableWhenPaused(true);
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void AMainGameMode_B::ResumeGame()
{

	PauseMenuWidget->RemoveFromViewport();
	if (PlayerControllerThatPaused)
	{
	FInputModeGameOnly InputMode;
	PlayerControllerThatPaused->SetInputMode(InputMode);
	}
	SetActorTickEnabled(false);
	SetTickableWhenPaused(true);
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}
