// Fill out your copyright notice in the Description page of Project Settings.


#include "LeaderFollower_B.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/LocalPlayer.h"
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
}

void ALeaderFollower_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float Height = GetBobbingHeight(GetGameTimeSinceCreation());
	if (LeadingPlayerController && IsValid(LeadingPlayerController->GetPawn()))
	{
		SetActorLocation(FMath::Lerp(GetActorLocation(), LeadingPlayerController->GetPawn()->GetActorLocation() + Offset + FVector(0.f,0.f, Height +BobAmplitude), LerpAlpha));
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), LeadingPlayerController->GetPawn()->GetActorRotation(), DeltaTime, LerpAlpha));
	}
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
				LeadingPlayerController = FoundLeadingPlayerController[0];
			}
		}
		else
		{
			Mesh->SetHiddenInGame(true);
		}
	}
}

float ALeaderFollower_B::GetBobbingHeight(float Time)
{
	return 	BobAmplitude * FMath::Sin(Time * BobFrequency);
}
