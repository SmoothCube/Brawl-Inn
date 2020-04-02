// Fill out your copyright notice in the Description page of Project Settings.


#include "VictoryGameMode_B.h"

#include "BrawlInn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Camera/CameraActor.h"
#include "Engine/LocalPlayer.h"

#include "System/GameInstance_B.h"
#include "System/SubSystems/ScoreSubSystem_B.h"

void AVictoryGameMode_B::BeginPlay()
{
	Super::BeginPlay();

	ACameraActor* IntroCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), GameInstance->GetCameraSwapTransform());
	check(IsValid(IntroCamera));
	UpdateViewTargets(IntroCamera);

	BWarn("Scoreboard sorted by most score:");
	for (auto Info : GameInstance->GetPlayerInfos())
	{
		APlayerController* Controller = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), Info.ID);
		BWarn("Player Controller ID: %i with %i as score.", Info.ID, Controller->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().Score);
	}
}

void AVictoryGameMode_B::PostLevelLoad()
{
	checkf(IsValid(VictoryCamera_BP), TEXT("VictoryCamera_BP is not set! Make sure to set it in the blueprint!"));

	ACameraActor* VictoryCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), VictoryCamera_BP));

	UpdateViewTargets(VictoryCamera, 3);
}
