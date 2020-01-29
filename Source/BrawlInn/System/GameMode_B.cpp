// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Components/DecalComponent.h"
#include "BrawlInn.h"

#include "System/InitPawn_B.h"
#include "System/Camera/GameCamera_B.h"
#include "System/MainGameMode_B.h"
#include "Player/PlayerController_B.h"
#include "Player/PlayerCharacter_B.h"
#include "Player/RespawnPawn_B.h"

void AGameMode_B::BeginPlay()
{
	Super::BeginPlay();

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
		Pawn->Destroy();
		APlayerCharacter_B* Character = GetWorld()->SpawnActor<APlayerCharacter_B>(BP_PlayerCharacter, ShouldUseVector ? SpawnTransform : GetRandomSpawnTransform());
		PlayerController->Possess(Character);
		PlayerController->RespawnPawn = nullptr;
		PlayerController->PlayerCharacter = Character;
		AMainGameMode_B* MainMode = Cast<AMainGameMode_B>(this);
		if (MainMode)
		{
			USceneComponent* Mesh = Cast<USceneComponent>(Character->GetMesh());
			if (Mesh)
				MainMode->AddCameraFocusPoint(Mesh);
			else
				BWarn("Cannot Find Mesh");

		}
		UpdateViewTarget(PlayerController);
		SpawnCharacter_NOPARAM_D.Broadcast();
	}
}

void AGameMode_B::DespawnCharacter(APlayerController_B* PlayerController)
{
	APawn* Pawn = PlayerController->GetPawn();
	if (IsValid(Pawn))
	{
		Pawn->Destroy();
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