// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController_B.h"
#include "BrawlInn.h"
#include "Player/PlayerCharacter_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/HealthComponent_B.h"
#include "Components/CharacterSelectionComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "Engine/World.h"
#include "System/GameMode_B.h"
#include "System/MainGameMode_B.h"
#include "Kismet/GameplayStatics.h"
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
		InputComponent->SetTickableWhenPaused(true);

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
}
void APlayerController_B::MoveRight(float Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->InputVector.Y = Value;
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
	if (!PlayerCharacter)
		return;

	if (!PlayerCharacter->HoldComponent->IsHolding())
		PlayerCharacter->PunchButtonPressed();
	else
		PlayerCharacter->ThrowComponent->TryThrow();
}

void APlayerController_B::TakeOneDamage_Implementation()
{
	HealthComponent->TakeDamage(1);
}