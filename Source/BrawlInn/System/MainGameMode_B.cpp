// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode_B.h"
#include "Engine/World.h"
#include "System/GameCamera_B.h"
#include "Player/PlayerController_B.h"


void AMainGameMode_B::BeginPlay()
{
	Super::BeginPlay();

	/// Spawns and setups camera
	GameCamera = GetWorld()->SpawnActor<AGameCamera_B>(BP_GameCamera, FTransform());
	for (const auto& PlayerController : PlayerControllers)
		PlayerController->SetViewTargetWithBlend(GameCamera);

	SpawnCharacter_D.AddDynamic(this, &AMainGameMode_B::UpdateViewTarget);
	DespawnCharacter_D.AddDynamic(this, &AMainGameMode_B::UpdateViewTarget);
	
	
	/// Spawns characters for the players
	for (int i = 0; i < NumberOfPlayers; i++)
	{
		SpawnCharacter_D.Broadcast(Cast<APlayerController_B>(PlayerControllers[i]));
	}

}
void AMainGameMode_B::UpdateViewTarget(APlayerController_B* PlayerController)
{
	if (IsValid(GameCamera))
		PlayerController->SetViewTargetWithBlend(GameCamera);
}