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


// Sets default values
ALeaderFollower_B::ALeaderFollower_B()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
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

// Called every frame
void ALeaderFollower_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float Height = GetBobbingHeight(GetGameTimeSinceCreation());
	if (LeadingPlayerController)
		SetActorLocation(FMath::Lerp(GetActorLocation(), LeadingPlayerController->GetPawn()->GetActorLocation() + Offset + FVector(0.f, 0.f, Height + BobAmplitude), LerpAlpha));
}

void ALeaderFollower_B::ScoreUpdated()
{
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode && GameMode->ShouldUseScoreMultiplier())
	{
		AGamePlayerController_B* FoundLeadingPlayerController = GameMode->GetLeadingPlayerController();
		if (FoundLeadingPlayerController)
		{
			if (FoundLeadingPlayerController->GetPawn())
			{
				LeadingPlayerController = FoundLeadingPlayerController;
			}
		}
	}
}

float ALeaderFollower_B::GetBobbingHeight(float Time)
{
	return 	BobAmplitude * FMath::Sin(Time * BobFrequency);
}
