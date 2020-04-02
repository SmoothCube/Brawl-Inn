// Fill out your copyright notice in the Description page of Project Settings.


#include "VictoryGameMode_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Camera/CameraActor.h"


void AVictoryGameMode_B::BeginPlay()
{
	Super::BeginPlay();

	checkf(IsValid(IntroCamera_BP), TEXT("IntroCamera_BP is not set! Make sure to set it in the blueprint!"));

	ACameraActor* IntroCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), IntroCamera_BP));

	UpdateViewTargets(IntroCamera);
}

void AVictoryGameMode_B::PostLevelLoad()
{
	checkf(IsValid(VictoryCamera_BP), TEXT("VictoryCamera_BP is not set! Make sure to set it in the blueprint!"));

	ACameraActor* VictoryCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), VictoryCamera_BP));

	UpdateViewTargets(VictoryCamera, 3);
}
