// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

#include "System/InitPawn_B.h"
#include "System/GameCamera_B.h"
#include "Player/PlayerController_B.h"
#include "Player/PlayerCharacter_B.h"

void AGameMode_B::BeginPlay()
{
	/// Finds spawnpoints
	TArray<AActor*> tempSpawnpoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), tempSpawnpoints);
	for (const auto& point : tempSpawnpoints)
	{
		Spawnpoints.Add(Cast<APlayerStart>(point));
	}

	/// Creates all the playercontrollers
	uint8 CurrentPlayer = 0;
	TArray<APlayerController*> PlayerControllers;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerControllers.Add(PlayerController);

	while (CurrentPlayer < 3)
	{
		PlayerControllers.Add(UGameplayStatics::CreatePlayer(GetWorld()));
		CurrentPlayer++;
	}

	/// Spawns and setups camera
	GameCamera = GetWorld()->SpawnActor<AGameCamera_B>(BP_GameCamera, FTransform());
	PlayerController->SetViewTargetWithBlend(GameCamera);

	/// Bind delegates
	SpawnCharacter_D.AddDynamic(this, &AGameMode_B::SpawnCharacter);
	DespawnCharacter_D.AddDynamic(this, &AGameMode_B::DespawnCharacter);

	/// Spawns characters for the players
	for (int i = 0; i < NumberOfPlayers; i++)
	{
		SpawnCharacter_D.Broadcast(Cast<APlayerController_B>(PlayerControllers[i]));
	}

	GameModeInit_D.Broadcast();

	Super::BeginPlay();
}

void AGameMode_B::SpawnCharacter(APlayerController_B* PlayerController)
{
	APawn* Pawn = PlayerController->GetPawn();
	if (IsValid(Pawn))
	{
		Pawn->Destroy();
		APlayerCharacter_B* Character = GetWorld()->SpawnActor<APlayerCharacter_B>(BP_PlayerCharacter, GetRandomSpawnTransform());
		PlayerController->Possess(Character);
		PlayerController->PlayerCharacter = Character;
		if (IsValid(GameCamera))
			PlayerController->SetViewTargetWithBlend(GameCamera);
	}
}

void AGameMode_B::DespawnCharacter(APlayerController_B* PlayerController)
{
	APawn* Pawn = PlayerController->GetPawn();
	if (IsValid(Pawn))
	{
		Pawn->Destroy();
		AInitPawn_B* Character = GetWorld()->SpawnActor<AInitPawn_B>(AInitPawn_B::StaticClass(), GetRandomSpawnTransform());
		PlayerController->Possess(Character);
		PlayerController->PlayerCharacter = nullptr;
		if (IsValid(GameCamera))
			PlayerController->SetViewTargetWithBlend(GameCamera);
	}
}

FTransform AGameMode_B::GetRandomSpawnTransform()
{
	if (Spawnpoints.Num() == 0)
		return {};
	int index = FMath::RandRange(0, Spawnpoints.Num() - 1);
	return Spawnpoints[index]->GetActorTransform();
}
