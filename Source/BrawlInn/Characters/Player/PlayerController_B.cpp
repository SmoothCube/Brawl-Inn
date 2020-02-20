// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "BrawlInn.h"

#include "Characters/Player/PlayerCharacter_B.h"
#include "Characters/Player/RespawnPawn_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/HealthComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "System/GameModes/MainGameMode_B.h"
#include "System/GameModes/MenuGameMode_B.h"

APlayerController_B::APlayerController_B()
{
	HealthComponent = CreateDefaultSubobject<UHealthComponent_B>("Health Component");
}

void APlayerController_B::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameOnly());
	bAutoManageActiveCameraTarget = false;
	HealthComponent->RespawnIsZero_D.AddLambda([&]() { bCanRespawn = false; });
}

void APlayerController_B::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent != nullptr)
	{
		// Movement bindings
		InputComponent->BindAxis("MoveUp", this, &APlayerController_B::MoveUp);
		InputComponent->BindAxis("MoveRight", this, &APlayerController_B::MoveRight);

		InputComponent->BindAction("Punch", IE_Pressed, this, &APlayerController_B::PunchButtonPressed);
		InputComponent->BindAction("Punch", IE_Released, this, &APlayerController_B::PunchButtonReleased);
		InputComponent->BindAction("Pickup", IE_Pressed, this, &APlayerController_B::PickupButtonPressed);
		InputComponent->BindAction("Pickup", IE_Repeat, this, &APlayerController_B::PickupButtonRepeat);

		InputComponent->BindAction("Pause", IE_Pressed, this, &APlayerController_B::TryPauseGame);
		InputComponent->BindAction("BreakFree", IE_Pressed, this, &APlayerController_B::BreakFreeButtonPressed);
		InputComponent->BindAction("Dash", IE_Pressed, this, &APlayerController_B::DashButtonPressed);

		// Debug
		InputComponent->BindAction("DEBUG_Spawn", IE_Pressed, this, &APlayerController_B::DEBUG_Spawn);
		InputComponent->BindAction("DEBUG_Despawn", IE_Pressed, this, &APlayerController_B::DEBUG_Despawn);
		InputComponent->BindAction("DEBUG_TEST01", IE_Pressed, this, &APlayerController_B::DEBUG_TEST01);
	}
}

void APlayerController_B::TryPauseGame()
{
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{

		GameMode->PauseGame(this);
	}
}

bool APlayerController_B::HasValidCharacter()
{
	if (PlayerCharacter)
		return true;
	return false;
}

void APlayerController_B::PlayControllerVibration(float Strength, float Duration, bool bAffectsLeftLarge, bool bAffectsLeftSmall, bool bAffectsRightLarge, bool bAffectsRightSmall, EDynamicForceFeedbackAction::Type Action)
{
	if (bVibrateControllers)
		PlayDynamicForceFeedback(Strength, Duration, bAffectsLeftLarge, bAffectsLeftSmall, bAffectsRightLarge, bAffectsRightSmall, Action);

}

void APlayerController_B::FellOutOfWorld(const UDamageType& dmgType)
{

}

void APlayerController_B::MoveUp(float Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->InputVector.X = Value;
	}
	else if (RespawnPawn)
	{
		RespawnPawn->InputVector.X = Value;
	}
}
void APlayerController_B::MoveRight(float Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->InputVector.Y = Value;
	}
	else if (RespawnPawn)
	{
		RespawnPawn->InputVector.Y = Value;
	}
}

void APlayerController_B::PickupButtonPressed()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->HoldComponent->TryPickup();
	}
}

void APlayerController_B::PickupButtonRepeat()
{
	if (PlayerCharacter && PlayerCharacter->HoldComponent)
		PlayerCharacter->HoldComponent->TryPickup();
}

void APlayerController_B::PunchButtonPressed()
{
	if (PlayerCharacter)
	{
		if (!PlayerCharacter->HoldComponent->IsHolding())
			PlayerCharacter->TryPunch();
		else
			PlayerCharacter->ThrowComponent->TryThrow();
	}
	else if (RespawnPawn)
	{
		RespawnPawn->ThrowBarrel();
	}
}

void APlayerController_B::PunchButtonReleased()
{

	if (PlayerCharacter &&
		PlayerCharacter->HoldComponent &&
		PlayerCharacter->ThrowComponent &&
		PlayerCharacter->HoldComponent->IsHolding())
	{
		PlayerCharacter->ThrowComponent->StartThrow();
	}
}

void APlayerController_B::BreakFreeButtonPressed()
{
	if (!PlayerCharacter)
		return;

	if (PlayerCharacter->GetState() == EState::EBeingHeld)
		PlayerCharacter->BreakFreeButtonMash();
}

void APlayerController_B::DashButtonPressed()
{
	if (!PlayerCharacter)
		return;

	if (PlayerCharacter->PunchComponent)
		PlayerCharacter->PunchComponent->Dash();
}

void APlayerController_B::Respawn()
{
	AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->RespawnCharacter_D.Broadcast(PlayerInfo);
	}
}

void APlayerController_B::TryRespawn(float RespawnDelay)
{
	if (bCanRespawn)
	{
		GetWorld()->GetTimerManager().SetTimer(TH_RespawnTimer, this, &APlayerController_B::Respawn, RespawnDelay, false);
	}
	else
	{
		AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
			GameMode->DespawnCharacter_D.Broadcast(this);
	}
}

void APlayerController_B::OnUnPossess()
{
	Super::OnUnPossess();
	PlayerCharacter = nullptr;
}

//Send in damage from 1-100
void APlayerController_B::TakeDamage_Implementation(int DamageAmount)
{
	HealthComponent->TakeDamage(DamageAmount);
}

void APlayerController_B::DEBUG_TEST01()
{
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->OnPlayerWin.Broadcast(this);
	}
}

void APlayerController_B::DEBUG_Spawn()
{
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
		GameMode->SpawnCharacter_D.Broadcast(PlayerInfo, false, FTransform());
}

void APlayerController_B::DEBUG_Despawn()
{
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
		GameMode->DespawnCharacter_D.Broadcast(this);
}
