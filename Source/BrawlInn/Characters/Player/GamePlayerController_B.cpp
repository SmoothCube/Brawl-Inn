// Fill out your copyright notice in the Description page of Project Settings.

#include "GamePlayerController_B.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "BrawlInn.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Characters/Player/RespawnPawn_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/GameMode_B.h"
#include "System/SubSystems/ScoreSubSystem_B.h"
#include "UI/UIElements/ColoredTextBlock_B.h"

void AGamePlayerController_B::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
}

void AGamePlayerController_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AGamePlayerController_B::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UGameInstance_B* GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	if (GameInstance)
	{
		//If sjekken er true om playerinfo ikke er hentet fra menumap
		if (PlayerInfo.CharacterVariant.bTaken == false)
		{
			PlayerInfo = GameInstance->GetPlayerInfo(UGameplayStatics::GetPlayerControllerID(this));
		}
		if (ScoreTextBlock)
			ScoreTextBlock->SetTextColor(PlayerInfo.CharacterVariant.TextColor);
	}
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
	if (TryBreakFree())
	{
	}
	else if (TryThrow())
	{
	}
	else if (TryStartPunchCharge())
	{
	}
	else if (RespawnPawn)
		RespawnPawn->ThrowBarrel();
}

void AGamePlayerController_B::FaceButtonTopReleased()
{
	if (!TryThrow())
		TryEndPunchCharge();
}

void AGamePlayerController_B::FaceButtonRightPressed()
{
	if (!TryBreakFree())
		TryDash();
}

void AGamePlayerController_B::FaceButtonBottomPressed()
{
	if (!TryBreakFree())
		TryPickup();
	if (RespawnPawn)
		RespawnPawn->ThrowBarrel();
}

void AGamePlayerController_B::FaceButtonBottomRepeat()
{
	TryPickup();
}

void AGamePlayerController_B::FaceButtonLeftPressed()
{
	if (!TryBreakFree())
		if (!TryThrow())
			TryPunch();
}

void AGamePlayerController_B::FaceButtonLeftReleased()
{
	TryThrow();
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
	if (!TryBreakFree())
		if (!TryThrow())
			TryPunch();
}

void AGamePlayerController_B::RightShoulderReleased()
{
	TryThrow();
}

void AGamePlayerController_B::RightTriggerPressed()
{
	if (TryBreakFree())
	{
	}
	else if (TryThrow())
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
		TryEndPunchCharge();
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
	if (IsValid(GameMode))
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
	if (PlayerCharacter &&
		PlayerCharacter->PunchComponent &&
		PlayerCharacter->PunchComponent->GetCanDash()
		)
		PlayerCharacter->PunchComponent->Dash();
}

bool AGamePlayerController_B::TryStartPunchCharge()
{
	if (PlayerCharacter &&
		PlayerCharacter->PunchComponent)
	{
		bPunchChargeInputHeld = true;
		if (PlayerCharacter->PunchComponent->GetCanPunch())
		{
			PlayerCharacter->TryStartCharging();
			return true;
		}
	}
	return false;
}

bool AGamePlayerController_B::TryEndPunchCharge()
{
	if (PlayerCharacter && PlayerCharacter->PunchComponent)
	{
		bPunchChargeInputHeld = false;
		if(PlayerCharacter->IsCharging() && PlayerCharacter->GetChargeLevel() > EChargeLevel::EChargeLevel1)
		{
			StopControllerVibration();
			PlayerCharacter->PunchComponent->SetIsPunching(true);
			PlayerCharacter->PunchComponent->SetCanPunch(false);
			PlayerCharacter->SetIsCharging(false);
			return true;
		}
	}
	return false;
}

bool AGamePlayerController_B::TryPunch()
{
	if (PlayerCharacter &&
		PlayerCharacter->HoldComponent &&
		!PlayerCharacter->HoldComponent->IsHolding() &&
		PlayerCharacter->PunchComponent &&
		PlayerCharacter->PunchComponent->GetCanPunch()
		)
	{
		PlayerCharacter->SetChargeLevel(EChargeLevel::EChargeLevel1);
		PlayerCharacter->PunchComponent->SetIsPunching(true);
		PlayerCharacter->PunchComponent->SetCanPunch(false);
		PlayerCharacter->SetIsCharging(false);	//do i need this?
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
		StopControllerVibration();
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
	check(IsValid(GameMode))
		GameMode->RespawnCharacter(PlayerInfo);
}

void AGamePlayerController_B::TryRespawn(const float ReSpawnDelay)
{
	if (bCanRespawn)
		GetWorld()->GetTimerManager().SetTimer(TH_RespawnTimer, this, &AGamePlayerController_B::Respawn, ReSpawnDelay, false);
}

void AGamePlayerController_B::SetScoreTextBlock(UColoredTextBlock_B* TextBlock)
{
	ScoreTextBlock = TextBlock;
	UScoreSubSystem_B* ScoreSubSystem = GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>();
	if (!ScoreSubSystem->OnScoreChanged.IsBoundToObject(TextBlock))
		ScoreSubSystem->OnScoreChanged.AddUObject(TextBlock, &UColoredTextBlock_B::UpdateScore);
}

bool AGamePlayerController_B::IsPunchChargeInputHeld()
{
	return bPunchChargeInputHeld;
}

void AGamePlayerController_B::Debug_Spawn() const
{
#if WITH_EDITOR
	check(IsValid(GameMode))
		GameMode->SpawnCharacter(PlayerInfo, false, FTransform());
#endif
}

void AGamePlayerController_B::Debug_DeSpawn()
{
#if WITH_EDITOR
	check(IsValid(GameMode))
		GameMode->DespawnCharacter(this);
#endif
}
