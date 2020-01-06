// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode_B.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Camera/CameraActor.h"
#include "System/InitPawn_B.h"
#include "Player/PlayerController_B.h"
#include "Player/PlayerCharacter_B.h"
#include "Engine/World.h"

void AGameMode_B::BeginPlay()
{
	/// Finds spawnpoints
	TArray<AActor*> tempSpawnpoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), tempSpawnpoints);
	for (const auto& point : tempSpawnpoints)
	{
		Spawnpoints.Add(Cast<APlayerStart>(point));
	}

	/// Creates players
	uint8 CurrentPlayer = 0;

	while (CurrentPlayer <= NumberOfPlayers - 2)
	{
		UGameplayStatics::CreatePlayer(GetWorld());
		CurrentPlayer++;

	}

	/// Bind delegates
	SpawnCharacter_D.AddDynamic(this, &AGameMode_B::SpawnCharacter);
	DespawnCharacter_D.AddDynamic(this, &AGameMode_B::DespawnCharacter);

	Super::BeginPlay();
}

void AGameMode_B::SpawnCharacter(APlayerController_B* PlayerController)
{
	APawn* Pawn = PlayerController->GetPawn();
	if (IsValid(Pawn))
	{
		Pawn->Destroy();
		APlayerCharacter_B* Character = GetWorld()->SpawnActor<APlayerCharacter_B>(PlayerToSpawn, GetRandomSpawnTransform());
		PlayerController->Possess(Character);
		PlayerController->PlayerCharacter = Character;
		if (GameCamera)
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
		if (GameCamera)
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
