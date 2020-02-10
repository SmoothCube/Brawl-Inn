// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "BrawlInn.h"

#include "System/Camera/GameCamera_B.h"
#include "Characters/Player/PlayerController_B.h"
#include "System/GameInstance_B.h"
#include "UI/Menus/PauseMenu_B.h"
#include "UI/Game/VictoryScreenWidget_B.h"
#include "UI/Game/GameOverlay_B.h"

void AMainGameMode_B::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);

	if (GameInstance && Birds)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Birds, 0.75 * GameInstance->MasterVolume * GameInstance->SfxVolume	,1.0f, FMath::FRandRange(0, 100));
	}
	if (GameInstance && River)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), River, 0.75 * GameInstance->MasterVolume * GameInstance->SfxVolume, 1.0f, FMath::FRandRange(0, 100));
	}

	/// Spawns and setups camera
	GameCamera = GetWorld()->SpawnActor<AGameCamera_B>(BP_GameCamera, FTransform());

	/// Create overlay
	UGameOverlay_B* Overlay = CreateWidget<UGameOverlay_B>(GetWorld(), BP_GameOverlay);
	Overlay->AddToViewport();

	/// Spawns characters for the players
	for (FPlayerInfo Info : Cast<UGameInstance_B>(GetGameInstance())->GetPlayerInfos())
	{
		APlayerController_B* PlayerController = Cast<APlayerController_B>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), Info.ID));
		if (!PlayerController) { BError("PlayerController for id %i not found. Check IDs in GameInstance", Info.ID); continue; }

		SpawnCharacter_D.Broadcast(Info,false,FTransform());

	}

	//Create and setup victory widget

	UVictoryScreenWidget_B* VictoryScreen = CreateWidget<UVictoryScreenWidget_B>(PlayerControllers[0], BP_VictoryScreen);
	VictoryScreen->AddToViewport();
	OnPlayerWin.AddUObject(VictoryScreen, &UVictoryScreenWidget_B::OnPlayerWin);

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
	else
		BWarn("Cannot Set view Target!");
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

void AMainGameMode_B::AddCameraFocusPoint(AActor* FocusActor)
{
	if (!IsValid(GameCamera) || !IsValid(FocusActor)) return;
	//TODO: check to see if they are inside the track box before adding.
	BWarn("Adding Actor %s to camera", *GetNameSafe(FocusActor));
	GameCamera->ActorsToTrack.Add(FocusActor);
}

void AMainGameMode_B::RemoveCameraFocusPoint(AActor* FocusActor)
{
	if (!IsValid(GameCamera) || !IsValid(FocusActor)) return;
	
	//Pretty sure its safe to do this even if it doesnt actally exist in the array.
	GameCamera->ActorsToTrack.Remove(FocusActor);

}