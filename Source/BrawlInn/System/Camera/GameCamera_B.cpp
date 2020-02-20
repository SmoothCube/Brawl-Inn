// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCamera_B.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/TriggerBox.h"
#include "Engine/LocalPlayer.h"

#include "BrawlInn.h"
#include "Characters/Player/PlayerController_B.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Components/SkeletalMeshComponent.h"

AGameCamera_B::AGameCamera_B()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = SceneComp;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(SceneComp);
	SpringArm->TargetArmLength = 2800;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 8.0;
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
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

	UpdateCameraPosition();
	SetSpringArmLength();

}

float AGameCamera_B::UpdateCameraPosition()
{
	if (!TrackingBox)
	{
		BError("CameraTrackingBox not found!"); return 0;
	}

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
	return distanceToFurthestPoint;
}

void AGameCamera_B::LerpCameraLocation(FVector LerpLoc)
{
	SetActorLocation(FMath::Lerp(GetActorLocation(), LerpLoc, LerpAlpha));
}

void AGameCamera_B::SetSpringArmLength()
{
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		LocalPlayer->ViewportClient->Viewport,
		GetWorld()->Scene,
		LocalPlayer->ViewportClient->EngineShowFlags)
		.SetRealtimeUpdate(true));

	FVector ViewLoc;
	FRotator ViewRot;
	FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, /*out*/ViewLoc, /*out*/ViewRot, LocalPlayer->ViewportClient->Viewport);

	float FurthestDist = -1000000;

	for (auto& a : ActorsToTrack)
	{
		FVector BorderVector = (a->GetActorLocation() - GetActorLocation()).GetSafeNormal() * BorderWidth;
		BorderVector.Z = 0;
		FVector TrackingPointWithBorder = a->GetActorLocation() + BorderVector;

		for (auto& p : SceneView->ViewFrustum.Planes)
		{
			float Dist = p.PlaneDot(TrackingPointWithBorder);
			if (Dist > 0 && Dist > FurthestDist)
				FurthestDist = Dist;
			else if (Dist < 0 && Dist > FurthestDist&& FurthestDist <= 0)
				FurthestDist = Dist;
		}
	}

	CameraZoom += FurthestDist * 0.1f;

	SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, CameraZoom, LerpAlpha);

	if (SpringArm->TargetArmLength <= SmallestSpringArmLength)
		SpringArm->TargetArmLength = SmallestSpringArmLength;
	//else if (SpringArm->TargetArmLength >= LargestSpringArmLength)
	//	SpringArm->TargetArmLength = LargestSpringArmLength;
	
}

void AGameCamera_B::OnTrackingBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
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
