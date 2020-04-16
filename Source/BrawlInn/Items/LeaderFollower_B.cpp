// Fill out your copyright notice in the Description page of Project Settings.


#include "LeaderFollower_B.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/LocalPlayer.h"
#include "TimerManager.h"
#include "Engine/World.h"

#include "BrawlInn.h"
#include "Characters/Player/GamePlayerController_B.h"
#include "System/GameModes/MainGameMode_B.h"
#include "System/SubSystems/ScoreSubSystem_B.h"

ALeaderFollower_B::ALeaderFollower_B()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALeaderFollower_B::BeginPlay()
{
	Super::BeginPlay();
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode && GameMode->ShouldUseScoreMultiplier())
	{
		GameMode->OnAnyScoreChange.AddUObject(this, &ALeaderFollower_B::ScoreUpdated);
	}
	ScoreUpdated();

	GetWorld()->GetTimerManager().SetTimer(CrownTimer, this, &ALeaderFollower_B::IncreaseCrownTime, 1.f, true);
}

void ALeaderFollower_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float Height = GetBobbingHeight(GetGameTimeSinceCreation());
	if (LeadingPlayerController && IsValid(LeadingPlayerController->GetPawn()))
	{
		APawn* PlayerPawn = LeadingPlayerController->GetPawn();
		SetActorLocation(
			FMath::Lerp(
				GetActorLocation(),
				PlayerPawn->GetActorLocation() + PlayerPawn->GetActorForwardVector().ToOrientationRotator().RotateVector(Offset) + FVector(0.f, 0.f, Height + BobAmplitude),
				LerpAlpha));
		AddActorLocalRotation(FRotator(0.f, RotationSpeed, 0.f));
	}
}

void ALeaderFollower_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	BLog("Adding Crowntime %i", CurrentCrownTime);

	if (LeadingPlayerController && LeadingPlayerController->GetLocalPlayer() && LeadingPlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>())
		LeadingPlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddScore(CurrentCrownTime, CrownTime); //the engine crashed here when I pressed escape once

	GetWorld()->GetTimerManager().ClearTimer(CrownTimer);
	Super::EndPlay(EndPlayReason);
}

void ALeaderFollower_B::ScoreUpdated()
{
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode && GameMode->ShouldUseScoreMultiplier())
	{
		auto FoundLeadingPlayerController = GameMode->GetLeadingPlayerController();

		if (FoundLeadingPlayerController.Num() == 1)
		{
			if (FoundLeadingPlayerController[0]->GetPawn())
			{
				Mesh->SetHiddenInGame(false);
				if ((FoundLeadingPlayerController[0] != LeadingPlayerController) && LeadingPlayerController)
				{
					LeadingPlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddScore(CurrentCrownTime, CrownTime);
				}
				CurrentCrownTime = 0;
				LeadingPlayerController = FoundLeadingPlayerController[0];
			}
		}
		else
		{
			Mesh->SetHiddenInGame(true);
		}
	}
}


void ALeaderFollower_B::IncreaseCrownTime()
{
	CurrentCrownTime++;
}

float ALeaderFollower_B::GetBobbingHeight(float Time)
{
	return 	BobAmplitude * FMath::Sin(Time * BobFrequency);
}
