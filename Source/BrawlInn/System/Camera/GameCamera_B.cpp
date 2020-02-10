// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCamera_B.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/TriggerBox.h"
#include "BrawlInn.h"

#include "Characters/Player/PlayerController_B.h"
#include "Characters/Player/PlayerCharacter_B.h"
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
	//VectorsToTrack.Add(new FVector(-1151.382568, 513.574097, 162.623901));
}

void AGameCamera_B::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "CameraFocus", ActorsToTrack);

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ATriggerBox::StaticClass(), "Camera", Actors);
	TrackingBox = Cast<ATriggerBox>(Actors[0]);
	if (TrackingBox)
	{
		TrackingBox->GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &AGameCamera_B::OnTrackingBoxEndOverlap);
		TrackingBox->GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AGameCamera_B::OnTrackingBoxBeginOverlap);
	}
}

void AGameCamera_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCamera();

}

void AGameCamera_B::LerpCameraLocation(FVector LerpLoc)
{
	SetActorLocation(FMath::Lerp(GetActorLocation(), LerpLoc, LerpAlpha));
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
	float distanceToFurthestPoint = 0.f;
	int ActiveFocusPoints = 0;

	//Cleanup in case some components dont get removed properly
	TArray<AActor*> ActorsToRemove;

	for (const auto& Actor : ActorsToTrack)
	{
		if (!IsValid(Actor))
		{
			BWarn("Invalid component in camera!");
			ActorsToRemove.Add(Actor);
			continue;
		}

		FVector FocusLocation = Actor->GetActorLocation();
		//BWarn("Focus Location: %s", *FocusLocation.ToString())
		sum += FocusLocation;
		ActiveFocusPoints++;

		float distance = FVector::Dist(FocusLocation, GetActorLocation());
		if (distance > distanceToFurthestPoint)
			distanceToFurthestPoint = distance;

	}
	for (auto& Actor : ActorsToRemove)
		ActorsToTrack.Remove(Actor);

	if (ActiveFocusPoints != 0)
		sum /= ActiveFocusPoints;

	if (sum.Z < MinCameraHeight)
		sum.Z = MinCameraHeight;
	else if (sum.Z > MaxCameraHeight)
		sum.Z = MaxCameraHeight;

	FHitResult OutHit; 
	LerpCameraLocation(sum);

	//BWarn("Sum: %s, %i", *sum.ToString(), (SetActorLocation(sum, false, &OutHit, ETeleportType::None)))

	SetSpringArmLength(distanceToFurthestPoint);
}

void AGameCamera_B::SetSpringArmLength(float distanceToFurthestPlayer)
{
	float longestVector = 0.f;

	for (int i = 0; i < ActorsToTrack.Num(); i++)
	{
		for (int j = i + 1; j < ActorsToTrack.Num(); j++)
		{
			if (ActorsToTrack.IsValidIndex(i) && ActorsToTrack.IsValidIndex(j))
			{
				float Temp = FMath::Abs((ActorsToTrack[i]->GetActorLocation() - ActorsToTrack[j]->GetActorLocation()).X);
				if (longestVector < Temp)
					longestVector = Temp;
			}
		}
	}
	float OffsetX = longestVector;
	float ActualBorderWidth = BorderWidth + OffsetX;

	float newTargetLength = distanceToFurthestPlayer + ActualBorderWidth;
	SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, newTargetLength, LerpAlpha);

	if (SpringArm->TargetArmLength <= SmallestSpringArmLength)
		SpringArm->TargetArmLength = SmallestSpringArmLength;
	else if (SpringArm->TargetArmLength >= LargestSpringArmLength)
		SpringArm->TargetArmLength = LargestSpringArmLength;

	
}

void AGameCamera_B::OnTrackingBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//The right component doesnt neccesarily run on overlap end
	//GetComponents returns UActorComponent, which inherits from USceneComponent. Cast should always be safe. 
	ActorsToTrack.Remove(OtherActor);

	APlayerCharacter_B* Player = Cast<APlayerCharacter_B>(OtherActor);
	if (Player)
	{
		//Checks to see if the player is still overlapping. Same method as in DragArea
		TArray<AActor*> OverlappingActors;
		TrackingBox->GetOverlappingActors(OverlappingActors);
		if (OverlappingActors.Find(Player) != INDEX_NONE)
		{
			ActorsToTrack.Add(Player);
		}
	}

	//Logging the actors for debug purpouses
	for (auto& c : ActorsToTrack)
	{
		BWarn("Component in camera: %s, Owner: %s", *GetNameSafe(c), *GetNameSafe(c->GetOwner()))
	}

}

void AGameCamera_B::OnTrackingBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	//if (OtherActor->IsA(APlayerCharacter_B::StaticClass()))	
	//	ActorsToTrack.Add(OtherActor);
}
