// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode_B.h"
#include "Engine/World.h"
#include "BrawlInn.h"
#include "System/GameCamera_B.h"
#include "Player/PlayerController_B.h"
#include "System/GameInstance_B.h"
#include "Kismet/GameplayStatics.h"

void AMainGameMode_B::BeginPlay()
{
	Super::BeginPlay();

	/// Spawns and setups camera
	GameCamera = GetWorld()->SpawnActor<AGameCamera_B>(BP_GameCamera, FTransform());
	
	/// Spawns characters for the players
	for (int ID : Cast<UGameInstance_B>(GetGameInstance())->ActivePlayerControllerIDs)
	{
		APlayerController_B* PlayerController = Cast<APlayerController_B>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), ID));
		if (!PlayerController) { BError("PlayerController for id %i not found. Check IDs in GameInstance", ID); continue; }

		SpawnCharacter_D.Broadcast(PlayerController);

	}

}

void AMainGameMode_B::UpdateViewTarget(APlayerController_B* PlayerController)
{
	if (IsValid(GameCamera))
		PlayerController->SetViewTargetWithBlend(GameCamera);
}