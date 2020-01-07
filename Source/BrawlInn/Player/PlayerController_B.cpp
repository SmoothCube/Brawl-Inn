// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController_B.h"
#include "Player/PlayerCharacter_B.h"
void APlayerController_B::BeginPlay()
{
	SetInputMode(FInputModeGameOnly());
	PlayerCharacter = Cast<APlayerCharacter_B>(GetPawn());
}

void APlayerController_B::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent != nullptr)
	{
		// Movement bindings
		InputComponent->BindAxis("MoveUp", this, &APlayerController_B::MoveUp);
		InputComponent->BindAxis("MoveRight", this, &APlayerController_B::MoveRight);

		InputComponent->BindAxis("RotateX", this, &APlayerController_B::RotateX);
		InputComponent->BindAxis("RotateY", this, &APlayerController_B::RotateY);
	}
}

bool APlayerController_B::HasValidCharacter()
{
	if (PlayerCharacter)
			return true;
	return false;
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

void APlayerController_B::RotateX(float Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RotationVector.X = Value;
	}
}
void APlayerController_B::RotateY(float Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RotationVector.Y = Value;
	}
}