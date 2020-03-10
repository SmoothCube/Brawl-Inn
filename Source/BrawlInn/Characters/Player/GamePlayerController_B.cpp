// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlayerController_B.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/LocalPlayer.h"

#include "BrawlInn.h"
#include "System/GameModes/MainGameMode_B.h"
#include "Characters/Player/RespawnPawn_B.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "System/SubSystems/ScoreSubSystem_B.h"
#include "System/GameInstance_B.h"
#include "UI/Game/HealthWidget_B.h"


void AGamePlayerController_B::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance_B* GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	if(GameInstance)
	{
		PlayerInfo = GameInstance->GetPlayerInfo(UGameplayStatics::GetPlayerControllerID(this));
	}
}

void AGamePlayerController_B::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (Cast<APlayerCharacter_B>(InPawn) && HealthWidget)
		HealthWidget->PostInitialize(Cast<APlayerCharacter_B>(InPawn));
}

void AGamePlayerController_B::DPadUpPressed()
{
	Debug_Spawn();
}

void AGamePlayerController_B::DPadDownPressed()
{
	Debug_DeSpawn();
}

void AGamePlayerController_B::FaceButtonTopPressed()
{
	if (!TryBreakFree())
		TryPickup();
}

void AGamePlayerController_B::FaceButtonTopRepeat()
{
	TryPickup();
}

void AGamePlayerController_B::FaceButtonRightPressed()
{
	if (!TryBreakFree())
		TryDash();
}

void AGamePlayerController_B::FaceButtonBottomPressed()
{
	if (!TryBreakFree())
		TryDash();
}

void AGamePlayerController_B::FaceButtonLeftPressed()
{
	if (!TryBreakFree())
		;
	else if (!TryStartThrowCharge())
		;
	else if (TryStartPunchCharge())
		;
	else if (RespawnPawn)
		RespawnPawn->ThrowBarrel();
}

void AGamePlayerController_B::FaceButtonLeftReleased()
{
	if (!TryThrow())
		TryPunch();

}

void AGamePlayerController_B::SpecialRightPressed()
{
	TryPauseGame();
}

void AGamePlayerController_B::LeftShoulderPressed()
{
	if (!TryBreakFree())
		TryDash();
}

void AGamePlayerController_B::RightShoulderPressed()
{
	TryBreakFree();
}

void AGamePlayerController_B::RightTriggerPressed()
{
	if (TryBreakFree())
	{
	}
	else if (TryStartThrowCharge())
	{
	}
	else if (TryStartPunchCharge())
	{
	}
	else if (RespawnPawn)
		RespawnPawn->ThrowBarrel();
}

void AGamePlayerController_B::RightTriggerReleased()
{
	if (!TryThrow())
		TryPunch();
}

void AGamePlayerController_B::LeftTriggerPressed()
{
	if (!TryBreakFree())
		TryPickup();
}

void AGamePlayerController_B::LeftTriggerRepeat()
{
	if (PlayerCharacter && PlayerCharacter->HoldComponent)
		PlayerCharacter->HoldComponent->TryPickup();
}

void AGamePlayerController_B::LeftStickXAxis(const float Value)
{
	if (PlayerCharacter)
		PlayerCharacter->SetInputVectorY(Value);
	else if (RespawnPawn)
		RespawnPawn->SetInputVectorY(Value);
}

void AGamePlayerController_B::LeftStickYAxis(const float Value)
{
	if (PlayerCharacter)
		PlayerCharacter->SetInputVectorX(Value);
	else if (RespawnPawn)
		RespawnPawn->SetInputVectorX(Value);
}

void AGamePlayerController_B::TryPauseGame()
{
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
		GameMode->PauseGame(this);
}

bool AGamePlayerController_B::TryBreakFree()
{
	if (PlayerCharacter && (PlayerCharacter->GetState() == EState::EBeingHeld))
	{
		PlayerCharacter->BreakFreeButtonMash();
		return true;
	}
	return false;
}

void AGamePlayerController_B::TryDash()
{
	if (PlayerCharacter && PlayerCharacter->PunchComponent)
		PlayerCharacter->PunchComponent->Dash();
}

bool AGamePlayerController_B::TryStartPunchCharge()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->TryPunch();
		return true;
	}
	return false;
}

bool AGamePlayerController_B::TryStartThrowCharge()
{
	if (PlayerCharacter && PlayerCharacter->HoldComponent->IsHolding())
	{
		PlayerCharacter->ThrowComponent->TryThrow();
		return true;
	}
	return false;
}

bool AGamePlayerController_B::TryPunch()
{
	if (PlayerCharacter &&
		PlayerCharacter->PunchComponent &&
		PlayerCharacter->IsCharging())
	{
		PlayerCharacter->PunchComponent->bIsPunching = true;
		PlayerCharacter->SetIsCharging(false);
		return true;
	}
	return false;
}

bool AGamePlayerController_B::TryThrow()
{
	if (PlayerCharacter &&
		PlayerCharacter->HoldComponent &&
		PlayerCharacter->ThrowComponent &&
		PlayerCharacter->HoldComponent->IsHolding())
	{
		PlayerCharacter->ThrowComponent->StartThrow();
		return true;
	}
	return false;
}

void AGamePlayerController_B::TryPickup()
{
	if (PlayerCharacter && PlayerCharacter->HoldComponent)
		PlayerCharacter->HoldComponent->TryPickup();
}

void AGamePlayerController_B::Respawn() const
{
	AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
		GameMode->RespawnCharacter_D.Broadcast(PlayerInfo);
}

void AGamePlayerController_B::TryRespawn(const float ReSpawnDelay)
{
	if (bCanRespawn)
		GetWorld()->GetTimerManager().SetTimer(TH_RespawnTimer, this, &AGamePlayerController_B::Respawn, ReSpawnDelay, false);
}

void AGamePlayerController_B::SetHealthWidget(UHealthWidget_B* Widget)
{
	HealthWidget = Widget;
	UScoreSubSystem_B* ScoreSubSystem = GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>();
	if (!ScoreSubSystem->OnScoreValuesChanged.IsBoundToObject(Widget))
	{
		ScoreSubSystem->OnScoreValuesChanged.AddUObject(Widget, &UHealthWidget_B::UpdateScoreValues);
	}
}

void AGamePlayerController_B::Debug_Spawn() const
{
#if WITH_EDITOR
	BScreen("Editor");
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
		GameMode->SpawnCharacter_D.Broadcast(PlayerInfo, false, FTransform());
#endif
}

void AGamePlayerController_B::Debug_DeSpawn()
{
#if WITH_EDITOR
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
		GameMode->DespawnCharacter_D.Broadcast(this);
#endif
}
