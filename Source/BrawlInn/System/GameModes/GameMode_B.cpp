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
		UGameplayStatics::PlaySound2D(GetWorld(), Music, 0.75 * GameInstance->MasterVolume * GameInstance->MusicVolume);
	}

	/// Finds spawnpoints
	GetAllSpawnpointsInWorld();

	/// Creates all playercontrollers
	CreatePlayerControllers();

	/// Bind delegates
	SpawnCharacter_D.AddUObject(this, &AGameMode_B::SpawnCharacter);
	RespawnCharacter_D.AddUObject(this, &AGameMode_B::RespawnCharacter);

	DespawnCharacter_D.AddUObject(this, &AGameMode_B::DespawnCharacter);

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
void AGameMode_B::SpawnCharacter(FPlayerInfo PlayerInfo, bool ShouldUseVector, FTransform SpawnTransform)
{
	APlayerController_B* PlayerController = Cast<APlayerController_B>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), PlayerInfo.ID));
	if (!PlayerController) { BError("Can't find the PlayerController!"); return; }
	APawn* Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn)) { BError("Can't find Pawn!"); return; }

	if (Pawn->IsA(AInitPawn_B::StaticClass()))
	{
		GameInstance->AddPlayerInfo(PlayerInfo);
	}
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Pawn->Destroy();
	if (BP_PlayerCharacters.Num() == 0) { BError("GameMode has no BP_PlayerCharacter!");  return; }
	int Index = (int)PlayerInfo.Type;
	if (!BP_PlayerCharacters.IsValidIndex(Index)) { BError("GameMode has no PlayerCharacter on Index %i", Index); return; }

	APlayerCharacter_B* Character = GetWorld()->SpawnActor<APlayerCharacter_B>(BP_PlayerCharacters[Index], ShouldUseVector ? SpawnTransform : GetRandomSpawnTransform(), params);
	PlayerController->Possess(Character);
	PlayerController->RespawnPawn = nullptr;
	PlayerController->PlayerCharacter = Character;
	AMainGameMode_B* MainMode = Cast<AMainGameMode_B>(this);
	if (MainMode && Character)
	{
		MainMode->AddCameraFocusPoint(Character);
	}
	UpdateViewTarget(PlayerController);
	SpawnCharacter_NOPARAM_D.Broadcast();
}

// N�r man respawner gjennom en barrel
void AGameMode_B::RespawnCharacter(FPlayerInfo PlayerInfo)
{
	APlayerController_B* PlayerController = Cast<APlayerController_B>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), PlayerInfo.ID));
	if (!PlayerController) { BError("Can't find the PlayerController!"); return; }
	APawn* Pawn = PlayerController->GetPawn();
	if (IsValid(Pawn))
		Pawn->Destroy();

	ARespawnPawn_B* RespawnPawn = GetWorld()->SpawnActor<ARespawnPawn_B>(BP_RespawnPawn, GetRandomSpawnTransform());
	PlayerController->Possess(RespawnPawn);
	PlayerController->PlayerCharacter = nullptr;
	PlayerController->RespawnPawn = RespawnPawn;
	AMainGameMode_B* MainMode = Cast<AMainGameMode_B>(this);
	if (MainMode)
 	    MainMode->AddCameraFocusPoint(RespawnPawn);
	
	UpdateViewTarget(PlayerController);
	OnRespawnCharacter_D.Broadcast();
}

void AGameMode_B::DespawnCharacter(APlayerController_B* PlayerController)
{
	if (!PlayerController) { BError("Can't find the PlayerController!"); return; }
	APawn* Pawn = PlayerController->GetPawn();
	if (IsValid(Pawn))
		Pawn->Destroy();

	AInitPawn_B* Character = GetWorld()->SpawnActor<AInitPawn_B>(AInitPawn_B::StaticClass(), GetRandomSpawnTransform());
	PlayerController->Possess(Character);
	PlayerController->PlayerCharacter = nullptr;
	if (GameInstance)
		GameInstance->RemovePlayerInfo(UGameplayStatics::GetPlayerControllerID(PlayerController));

	UpdateViewTarget(PlayerController);
	DespawnCharacter_NOPARAM_D.Broadcast();
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