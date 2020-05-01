// Fill out your copyright notice in the Description page of Project Settings.


#include "LeaderFollower_B.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/LocalPlayer.h"

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
		GameMode->OnAnyScoreChange().AddUObject(this, &ALeaderFollower_B::ScoreUpdated);
	}
	ScoreUpdated();
}

void ALeaderFollower_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentCrownTime += DeltaTime;
	
	const float Height = GetBobbingHeight(GetGameTimeSinceCreation());
	if (LeadingPlayerController && IsValid(LeadingPlayerController->GetPawn()))
	{
		APawn* PlayerPawn = LeadingPlayerController->GetPawn();
		SetActorLocation(
			FMath::Lerp(
				GetActorLocation(),
				PlayerPawn->GetActorLocation()
				+ PlayerPawn->GetActorForwardVector().ToOrientationRotator().RotateVector(Offset)
				+ FVector(0.f, 0.f, AnimationZOffset)
				+ FVector(0.f, 0.f, Height + BobAmplitude),
				LerpAlpha));
		AddActorLocalRotation(FRotator(0.f, RotationSpeed+ AnimationRotationIncrease, 0.f));
	}
}

void ALeaderFollower_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	if (LeadingPlayerController && LeadingPlayerController->GetLocalPlayer() && LeadingPlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>())
		LeadingPlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddStats(static_cast<int>(CurrentCrownTime), CrownTime); //the engine crashed here when I pressed escape once

	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode && GameMode->ShouldUseScoreMultiplier())
	{
		GameMode->OnAnyScoreChange().RemoveAll(this);
	}
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
				if ((FoundLeadingPlayerController[0] != LeadingPlayerController))
				{
					if(LeadingPlayerController)
						LeadingPlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddStats(static_cast<int>(CurrentCrownTime), CrownTime);
					NextControllerToFollow = FoundLeadingPlayerController[0];
					Dissapear();
					CurrentCrownTime = 0;
				}
			}
		}
		else
		{
			NextControllerToFollow = nullptr;
			Dissapear();
		}
	}
}

float ALeaderFollower_B::GetBobbingHeight(float Time)
{
	return 	BobAmplitude * FMath::Sin(Time * BobFrequency);
}

void ALeaderFollower_B::AnimationTimelineFinished(bool bAppeared)
{
	//if dissapeared, swap leader and appear
	if (!bAppeared)
	{
		if (NextControllerToFollow)
		{
			LeadingPlayerController = NextControllerToFollow;
			NextControllerToFollow = nullptr;
			Appear();
		}
		else
		{
			LeadingPlayerController = nullptr;
		}
	}
}
