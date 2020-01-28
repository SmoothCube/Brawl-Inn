// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCamera_B.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BrawlInn.h"
#include "Engine/TriggerBox.h"

#include "Player/PlayerController_B.h"
#include "Player/PlayerCharacter_B.h"
#include "Components/SkeletalMeshComponent.h"

AGameCamera_B::AGameCamera_B()
{
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

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ATriggerBox::StaticClass(), "Camera", Actors);
	TrackingBox = Cast<ATriggerBox>(Actors[0]);
}

void AGameCamera_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCamera();

}

void AGameCamera_B::UpdateCamera()
{
	if (!TrackingBox)
	{
		BWarn("CameraTrackingBox not found!");
		return;
	}

	TArray<AActor*> TempArray;
	TrackingBox->GetOverlappingActors(TempArray, APlayerCharacter_B::StaticClass());

	FVector sum = FVector::ZeroVector;
	float distanceToFurthestPlayer = 0.f;
	int ActivePlayers = 0;
	PlayerCharacters.Empty();

	for (const auto& Actor : TempArray)
	{
		APlayerCharacter_B* Character = Cast<APlayerCharacter_B>(Actor);
		if (!Character || !Character->GetPlayerController_B())
			continue;

		if (Character->GetPlayerController_B())
		{
			PlayerCharacters.Add(Character);
		}

		FVector PlayerMeshLocation = Character->GetMesh()->GetComponentLocation();
		sum += PlayerMeshLocation;
		ActivePlayers++;

		float distance = FVector::Dist(PlayerMeshLocation, GetActorLocation());
		if (distance > distanceToFurthestPlayer)
			distanceToFurthestPlayer = distance;

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

	for (int i = 0; i < PlayerCharacters.Num(); i++)
	{
		for (int j = i + 1; j < PlayerCharacters.Num(); j++)
		{
			if (PlayerCharacters.IsValidIndex(i) && PlayerCharacters.IsValidIndex(j))
			{
				float Temp = FMath::Abs((PlayerCharacters[i]->GetMesh()->GetComponentLocation() - PlayerCharacters[j]->GetMesh()->GetComponentLocation()).X);
				if (longestVector < Temp)
					longestVector = Temp;
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
