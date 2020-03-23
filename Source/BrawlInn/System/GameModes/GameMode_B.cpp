// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode_B.h"
#include "Camera/CameraActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

#include "BrawlInn.h"
#include "Characters/Player/GamePlayerController_B.h"
#include "Characters/Player/InitPawn_B.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Characters/Player/RespawnPawn_B.h"
#include "System/Camera/GameCamera_B.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/MainGameMode_B.h"

void AGameMode_B::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());

	/// Finds spawnpoints
	GetAllSpawnpointsInWorld();

	/// Creates all playercontrollers
	CreatePlayerControllers();

	/// Bind delegates
	SpawnCharacter_D.AddUObject(this, &AGameMode_B::SpawnCharacter);
	RespawnCharacter_D.AddUObject(this, &AGameMode_B::RespawnCharacter);

	DespawnCharacter_D.AddUObject(this, &AGameMode_B::DespawnCharacter);
}

void AGameMode_B::DisableControllerInputs()
{
	for (auto Controller : PlayerControllers)
		Controller->DisableInput(Controller);
}

void AGameMode_B::EnableControllerInputs()
{
	for (auto Controller : PlayerControllers)
		Controller->EnableInput(Controller);
}

void AGameMode_B::UpdateViewTargets(ACameraActor* Camera, float BlendTime, bool LockOutgoing)
{
	AActor* CameraToUse = GameCamera;
	if (IsValid(Camera))
		CameraToUse = Camera;

	for (auto& PlayerController : PlayerControllers)
	{
		if (IsValid(CameraToUse))
			PlayerController->SetViewTargetWithBlend(CameraToUse, BlendTime, EViewTargetBlendFunction::VTBlend_EaseInOut,2, LockOutgoing);
	}
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
		PlayerControllers.Add(Cast<AGamePlayerController_B>(Actor));
}

// ---------------- Spawn PlayerCharacter functions --------------------------
void AGameMode_B::SpawnCharacter(FPlayerInfo PlayerInfo, bool ShouldUseVector, FTransform SpawnTransform)
{
	AGamePlayerController_B* PlayerController = Cast<AGamePlayerController_B>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), PlayerInfo.ID));
	if (!PlayerController) { BError("Can't find the PlayerController!"); return; }
	APawn* Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn)) { BError("Can't find Pawn!"); return; }

	if (Pawn->IsA(AInitPawn_B::StaticClass()))
	{
		GameInstance->AddPlayerInfo(PlayerInfo);
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Pawn->Destroy();
	if (BP_PlayerCharacters.Num() == 0) { BError("GameMode has no BP_PlayerCharacter!");  return; }
	const int Index = (int)PlayerInfo.Type;
	if (!BP_PlayerCharacters.IsValidIndex(Index)) { BError("GameMode has no PlayerCharacter on Index %i", Index); return; }

	APlayerCharacter_B* Character = nullptr;
	if (PlayerInfo.CharacterVariant.bTaken)
	{
		Character = GetWorld()->SpawnActor<APlayerCharacter_B>(BP_PlayerCharacters[Index], ShouldUseVector ? SpawnTransform : GetSpawnTransform(PlayerInfo.ID), Params);
		Character->GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(1, PlayerInfo.CharacterVariant.MeshMaterial);
		Character->GetDirectionIndicatorPlane()->CreateAndSetMaterialInstanceDynamicFromMaterial(0, PlayerInfo.CharacterVariant.IndicatorMaterial);
	}
	else
	{
		Character = GetWorld()->SpawnActor<APlayerCharacter_B>(BP_PlayerCharacters[Index], ShouldUseVector ? SpawnTransform : GetSpawnTransform(PlayerInfo.ID), Params);
	}

	PlayerController->Possess(Character);
	PlayerController->RespawnPawn = nullptr;
	PlayerController->SetPlayerCharacter(Character);
	AMainGameMode_B* MainMode = Cast<AMainGameMode_B>(this);
	if (MainMode && Character)
	{
		MainMode->AddCameraFocusPoint(Character);
	}
	UpdateViewTargets();
	SpawnCharacter_NOPARAM_D.Broadcast();
}

// Når man respawner gjennom en barrel
void AGameMode_B::RespawnCharacter(FPlayerInfo PlayerInfo)
{
	AGamePlayerController_B* PlayerController = Cast<AGamePlayerController_B>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), PlayerInfo.ID));
	if (!PlayerController) { BError("Can't find the PlayerController!"); return; }
	APawn* Pawn = PlayerController->GetPawn();
	if (IsValid(Pawn))
		Pawn->Destroy();

	TArray<AActor*> SpawnActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "RespawnStart", SpawnActors);
	ARespawnPawn_B* RespawnPawn;
	if (SpawnActors.IsValidIndex(0) && IsValid(SpawnActors[0]))
		RespawnPawn = GetWorld()->SpawnActor<ARespawnPawn_B>(BP_RespawnPawn, SpawnActors[0]->GetActorTransform());
	else
		RespawnPawn = GetWorld()->SpawnActor<ARespawnPawn_B>(BP_RespawnPawn, GetRandomSpawnTransform());
	if (RespawnPawn)
	{
		PlayerController->Possess(RespawnPawn);
		PlayerController->RespawnPawn = RespawnPawn;
		AMainGameMode_B* MainMode = Cast<AMainGameMode_B>(this);
		if (MainMode)
			MainMode->AddCameraFocusPoint(RespawnPawn);
	}

	PlayerController->SetPlayerCharacter(nullptr);
	UpdateViewTargets();
	OnRespawnCharacter_D.Broadcast();
}

void AGameMode_B::DespawnCharacter(AGamePlayerController_B* PlayerController)
{
	if (!PlayerController) { BError("Can't find the PlayerController!"); return; }
	APawn* Pawn = PlayerController->GetPawn();
	if (IsValid(Pawn))
		Pawn->Destroy();

	AInitPawn_B* Character = GetWorld()->SpawnActor<AInitPawn_B>(AInitPawn_B::StaticClass(), FTransform());
	PlayerController->Possess(Character);
	PlayerController->SetPlayerCharacter(nullptr);
	if (GameInstance)
		GameInstance->RemovePlayerInfo(UGameplayStatics::GetPlayerControllerID(PlayerController));

	UpdateViewTargets();
	DespawnCharacter_NOPARAM_D.Broadcast();
}

FTransform AGameMode_B::GetRandomSpawnTransform()
{
	if (Spawnpoints.Num() == 0) { BError("No spawnpoints found!"); return FTransform(); }
	return Spawnpoints[FMath::RandRange(0, Spawnpoints.Num() - 1)]->GetActorTransform();
}

FTransform AGameMode_B::GetSpawnTransform(const int PlayerControllerID)
{
	if (Spawnpoints.Num() == 0) { BError("No spawnpoints found!"); return FTransform(); }
	return Spawnpoints[PlayerControllerID]->GetActorTransform();
}

void AGameMode_B::GetAllSpawnpointsInWorld()
{
	Spawnpoints.Empty();
	TArray<AActor*> TempSpawnpoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), TempSpawnpoints);
	for (auto& Point : TempSpawnpoints)
		Spawnpoints.Add(Cast<APlayerStart>(Point));
}

AGameCamera_B* AGameMode_B::GetGameCamera() const
{
	return GameCamera;
}

void AGameMode_B::AddCameraFocusPoint(AActor* FocusActor)
{
	if (!IsValid(GameCamera) || !IsValid(FocusActor)) return;
	//TODO: check to see if they are inside the track box before adding.
	GameCamera->ActorsToTrack.Add(FocusActor);
}

void AGameMode_B::RemoveCameraFocusPoint(AActor* FocusActor)
{
	if (!IsValid(GameCamera) || !IsValid(FocusActor)) return;

	//Pretty sure its safe to do this even if it doesnt actally exist in the array.
	GameCamera->ActorsToTrack.Remove(FocusActor);

}
