// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerController_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "BrawlInn.h"

#include "Characters/Player/PlayerCharacter_B.h"
#include "Characters/Player/RespawnPawn_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/HealthComponent_B.h"
#include "Components/CharacterSelectionComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "System/GameMode_B.h"
#include "System/MainGameMode_B.h"
#include "System/MenuGameMode_B.h"

APlayerController_B::APlayerController_B()
{
	HealthComponent = CreateDefaultSubobject<UHealthComponent_B>("Health Component");

}

void APlayerController_B::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameOnly());
	HealthComponent->HealthIsZero_D.AddDynamic(this, &APlayerController_B::KillPlayerCharacter);
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
		InputComponent->BindAction("Pickup", IE_Pressed, this, &APlayerController_B::PickupButtonPressed);
		InputComponent->BindAction("Pickup", IE_Repeat, this, &APlayerController_B::PickupButtonRepeat);
		InputComponent->BindAction("SelectRight", IE_Pressed, this, &APlayerController_B::SelectRight);
		InputComponent->BindAction("Unselect", IE_Pressed, this, &APlayerController_B::Unselect);

		InputComponent->BindAction("Pause", IE_Pressed, this, &APlayerController_B::TryPauseGame);
		InputComponent->BindAction("BreakFree", IE_Pressed, this, &APlayerController_B::BreakFreeButtonPressed);

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

void APlayerController_B::SelectRight()
{
	if (!PlayerCharacter)
	{
		AMenuGameMode_B* GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
			GameMode->CharacterSelectionComponent->NextCharacter(this);
	}
}

void APlayerController_B::Unselect()
{
	AMenuGameMode_B* GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
		GameMode->CharacterSelectionComponent->Unselect(this);
}

// Called when the Health in HealthComponent is 0
void APlayerController_B::KillPlayerCharacter()
{
	AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->DespawnCharacter_D.Broadcast(this);
}

void APlayerController_B::PickupButtonPressed()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->HoldComponent->TryPickup();
	}
	else
	{
		AMenuGameMode_B* GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
			GameMode->CharacterSelectionComponent->SelectCharacter(this);
	}
}

void APlayerController_B::PickupButtonRepeat()
{
	PlayerCharacter->HoldComponent->TryPickup();
}

void APlayerController_B::PunchButtonPressed()
{
	if (PlayerCharacter)
	{
		if (!PlayerCharacter->HoldComponent->IsHolding())
			PlayerCharacter->PunchButtonPressed();
		else
			PlayerCharacter->ThrowComponent->TryThrow();
	}
	else if (RespawnPawn)
	{
		RespawnPawn->ThrowBarrel();
	}
}

void APlayerController_B::BreakFreeButtonPressed()
{
	if (!PlayerCharacter)
		return;

	if (PlayerCharacter->GetState() == EState::EBeingHeld)
		PlayerCharacter->BreakFreeButtonMash();
}

void APlayerController_B::TakeOneDamage_Implementation()
{
	HealthComponent->TakeDamage(1);
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
	{
		GameMode->SpawnCharacter_D.Broadcast(this, false, FTransform());
	}
}

void APlayerController_B::DEBUG_Despawn()
{
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->DespawnCharacter_D.Broadcast(this);
	}
}
