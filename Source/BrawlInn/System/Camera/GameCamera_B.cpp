// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCamera_B.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BrawlInn.h"

#include "Player/PlayerController_B.h"
#include "Player/PlayerCharacter_B.h"
#include "Components/SkeletalMeshComponent.h"
#include "System/Camera/CameraTrackingBox_B.h"

// Sets default values
AGameCamera_B::AGameCamera_B()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(Scene);
	SpringArm->TargetArmLength = 1000;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 8.0;
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
}

void AGameCamera_B::BeginPlay()
{
	Super::BeginPlay();

	/// Find all PlayerControllers and cache them. Uses PlayerControllers since characters havent spawned yet(?)
	TArray<AActor*> TempPlayerControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController_B::StaticClass(), TempPlayerControllers);
	for (const auto& Controller : TempPlayerControllers)
	{
		PlayerControllers.Add(Cast<APlayerController_B>(Controller));
	}

	//TrackingBox
	TrackingBox = Cast<ACameraTrackingBox_B>(UGameplayStatics::GetActorOfClass(GetWorld(), ACameraTrackingBox_B::StaticClass()));
//	Box->GetOverlappingActors()
}

void AGameCamera_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCamera();

}

void AGameCamera_B::UpdateCamera()
{
	if (PlayerControllers.Num() == 0)
		return;

	FVector sum = FVector::ZeroVector;
	float distanceToFurthestPlayer = 0.f;
	int ActivePlayers = 0;
	if (!TrackingBox)
	{
		BWarn("CameraTrackingBox not found!"); 
		return;
	}

	for (const auto Controller : PlayerControllers)
	{
		if (!Controller->HasValidCharacter())
		{
			//BWarn("Controller has no valid character!!");
			continue;
		}

		FVector PlayerMeshLocation = Controller->PlayerCharacter->GetMesh()->GetComponentLocation();
		sum += PlayerMeshLocation;
		ActivePlayers++;

		float distance = FVector::Dist(PlayerMeshLocation, GetActorLocation());
		if (distance > distanceToFurthestPlayer)
			distanceToFurthestPlayer = distance;

		//BWarn("PlayerMeshLocation: %s , distance: %f", *PlayerMeshLocation.ToString(), distance);

	}
	if (ActivePlayers != 0)
		sum /= ActivePlayers;

	if (sum.Z < MinCameraHeight)
		sum.Z = MinCameraHeight;
	else if (sum.Z > MaxCameraHeight)
		sum.Z = MaxCameraHeight;

	SetActorLocation(sum);

	SetSpringArmLength(distanceToFurthestPlayer);
}

void AGameCamera_B::SetSpringArmLength(float distanceToFurthestPlayer)
{
	float longestVector = 0.f;

	for (int i = 0; i < PlayerControllers.Num(); i++)
	{
		for (int j = i + 1; j < PlayerControllers.Num(); j++)
		{
			if (PlayerControllers.IsValidIndex(i) && PlayerControllers.IsValidIndex(j))
			{
				if (PlayerControllers[i]->HasValidCharacter() && PlayerControllers[j]->HasValidCharacter())
				{
					float Temp = FMath::Abs((PlayerControllers[i]->PlayerCharacter->GetMesh()->GetComponentLocation() - PlayerControllers[j]->PlayerCharacter->GetMesh()->GetComponentLocation()).X);
					if (longestVector < Temp)
						longestVector = Temp;
				}
			}
		}
	}
	float OffsetX = longestVector;
	float ActualBorderWidth = BorderWidth + OffsetX;

	float newTargetLength = distanceToFurthestPlayer + ActualBorderWidth;

	if (newTargetLength <= SmallestSpringArmLength)
		SpringArm->TargetArmLength = SmallestSpringArmLength;
	else if (newTargetLength >= LargestSpringArmLength)
		SpringArm->TargetArmLength = LargestSpringArmLength;
	else
		SpringArm->TargetArmLength = newTargetLength;
}
