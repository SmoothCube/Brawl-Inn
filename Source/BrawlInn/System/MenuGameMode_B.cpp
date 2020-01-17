// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameMode_B.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "Player/PlayerController_B.h"
#include "Camera/CameraActor.h"

void AMenuGameMode_B::BeginPlay()
{
	Super::BeginPlay();

}

void AMenuGameMode_B::UpdateViewTarget(APlayerController_B* PlayerController)
{
	if (IsValid(Camera))
		PlayerController->SetViewTargetWithBlend(Camera);
}

void AMenuGameMode_B::UpdateViewTargets()
{
	for (const auto& PlayerController : PlayerControllers)
	{
		if (IsValid(Camera))
			PlayerController->SetViewTargetWithBlend(Camera);
	}

}