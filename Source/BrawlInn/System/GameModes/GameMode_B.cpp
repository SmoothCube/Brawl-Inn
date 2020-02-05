// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Components/DecalComponent.h"
#include "Sound/SoundCue.h"
#include "BrawlInn.h"

#include "System/GameInstance_B.h"
#include "System/Camera/GameCamera_B.h"
#include "System/GameModes/MainGameMode_B.h"
#include "System/Camera/GameCamera_B.h"
#include "Characters/Player/InitPawn_B.h"
#include "Characters/Player/PlayerController_B.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Characters/Player/RespawnPawn_B.h"

void AGameMode_B::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	if (GameInstance && Music)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Music, 0.75* GameInstance->MasterVolume* GameInstance->MusicVolume);
	}
	
	/// Finds spawnpoints
	GetAllSpawnpointsInWorld();

	/// Creates all playercontrollers
	CreatePlayerControllers();

	/// Bind delegates
	SpawnCharacter_D.AddDynamic(this, &AGameMode_B::SpawnCharacter);
	DespawnCharacter_D.AddDynamic(this, &AGameMode_B::DespawnCharacter);

}

void AGameMode_B::CreatePlayerControllers()
{
	PlayerControllers.Empty();
	uint8 CurrentPlayer = GetNumPlayers();
	uint8 NumberOfPlayersCreated = 0;
	while (CurrentPlayer < 4)
	{
		UGameplayStatics::CreatePlayer(GetWorld());
		CurrentPlayer++;
		NumberOfPlayersCreated++;
	}
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController_B::StaticClass(), OutActors);
	for (const auto& Actor : OutActors)
		PlayerControllers.Add(Cast<APlayerController_B>(Actor));
}

// ---------------- Spawn PlayerCharacter functions --------------------------
void AGameMode_B::SpawnCharacter(APlayerController_B* PlayerController, bool ShouldUseVector, FTransform SpawnTransform)
{
	APawn* Pawn = PlayerController->GetPawn();
	if (IsValid(Pawn))
	{
		if (Pawn->IsA(AInitPawn_B::StaticClass()))
		{
			GameInstance->AddPlayerControllerID(UGameplayStatics::GetPlayerControllerID(PlayerController));
		}

		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		Pawn->Destroy();
		APlayerCharacter_B* Character = GetWorld()->SpawnActor<APlayerCharacter_B>(BP_PlayerCharacter, ShouldUseVector ? SpawnTransform : GetRandomSpawnTransform(), params);
		PlayerController->Possess(Character);
		PlayerController->RespawnPawn = nullptr;
		PlayerController->PlayerCharacter = Character;
		AMainGameMode_B* MainMode = Cast<AMainGameMode_B>(this);
		if (MainMode && Character)
		{
			USceneComponent* Mesh = Cast<USceneComponent>(Character->GetMesh()); //Character was nullptr here. how? 
			if (Mesh)
				MainMode->AddCameraFocusPoint(Mesh);
			else
				BWarn("Cannot Find Mesh");

		}
		UpdateViewTarget(PlayerController);
		SpawnCharacter_NOPARAM_D.Broadcast();
	}
}

void AGameMode_B::DespawnCharacter(APlayerController_B* PlayerController, bool bShouldRespawn)
{
	APawn* Pawn = PlayerController->GetPawn();
	if (IsValid(Pawn))
	{
		Pawn->Destroy();
		if (bShouldRespawn)
		{
			ARespawnPawn_B* RespawnPawn = GetWorld()->SpawnActor<ARespawnPawn_B>(BP_RespawnPawn, GetRandomSpawnTransform());
			PlayerController->Possess(RespawnPawn);
			PlayerController->PlayerCharacter = nullptr;
			PlayerController->RespawnPawn = RespawnPawn;
			AMainGameMode_B* MainMode = Cast<AMainGameMode_B>(this);
			if (MainMode)
			{
				USceneComponent* Decal = Cast<USceneComponent>(RespawnPawn->Decal);
				if (Decal)
					MainMode->AddCameraFocusPoint(Decal);
				else
					BWarn("Cannot Find Decal");

			}
		}
		else
		{
			AInitPawn_B* Character = GetWorld()->SpawnActor<AInitPawn_B>(AInitPawn_B::StaticClass(), GetRandomSpawnTransform());
			PlayerController->Possess(Character);
			PlayerController->PlayerCharacter = nullptr;
			if(GameInstance)
				GameInstance->RemovePlayerControllerID(UGameplayStatics::GetPlayerControllerID(PlayerController));
		}
		UpdateViewTarget(PlayerController);
		DespawnCharacter_NOPARAM_D.Broadcast();
	}
}

FTransform AGameMode_B::GetRandomSpawnTransform()
{
	if (Spawnpoints.Num() == 0) { BError("No spawnpoints found!"); return FTransform(); }
	return Spawnpoints[FMath::RandRange(0, Spawnpoints.Num() - 1)]->GetActorTransform();
}

void AGameMode_B::GetAllSpawnpointsInWorld()
{
	Spawnpoints.Empty();
	TArray<AActor*> tempSpawnpoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), tempSpawnpoints);
	for (const auto& point : tempSpawnpoints)
		Spawnpoints.Add(Cast<APlayerStart>(point));
}