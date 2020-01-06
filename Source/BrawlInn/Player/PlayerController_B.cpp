// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController_B.h"
#include "Player/PlayerCharacter_B.h"
void APlayerController_B::BeginPlay()
{
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
	}
}

void APlayerController_B::MoveUp(float Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->MoveUp(Value);
	}
}
void APlayerController_B::MoveRight(float Value)
{
	if (PlayerCharacter)
	{
		PlayerCharacter->MoveRight(Value);
	}
}
