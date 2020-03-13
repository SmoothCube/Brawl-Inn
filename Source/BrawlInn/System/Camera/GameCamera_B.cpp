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

	//Caches the camera rotation angle
	StartPitch = SpringArm->GetComponentRotation().Pitch;
}

void AGameCamera_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCameraPosition();
	SetSpringArmPitch();
	SetSpringArmLength();

}

void AGameCamera_B::UpdateCameraPosition()
{
	if (!TrackingBox)
	{
		BError("CameraTrackingBox not found!"); return;
	}

	FVector sum = FVector::ZeroVector;
	float distanceToFurthestPoint = 0.f;
	int ActiveFocusPoints = 0;

	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		LocalPlayer->ViewportClient->Viewport,
		GetWorld()->Scene,
		LocalPlayer->ViewportClient->EngineShowFlags)
		.SetRealtimeUpdate(true));

	FVector ViewLoc;
	FRotator ViewRot;
	FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, /*out*/ViewLoc, /*out*/ViewRot, LocalPlayer->ViewportClient->Viewport);
	if (!SceneView) { BWarn("Could not find scene view! "); return; }

	float FurthestDist = -1000000;
	float size = 0.1;
	int CurrentPlane = 0;

	//Cleanup in case some components dont get removed properly
	TArray<AActor*> ActorsToRemove;
	for (int i=0; i< SceneView->ViewFrustum.Planes.Num(); i++)
	{
		
		auto& p = SceneView->ViewFrustum.Planes[i];
		float DistOutside = 0	;
		float DistInside = -10000000;
		p.GetSafeNormal();
		FVector FurthestVec = FVector::ZeroVector;
		FVector ClosestVec = FVector::ZeroVector;
		int CurrentActor = 0;

		FVector DirVec = FVector::ZeroVector;
		switch (i)
		{
		case 0:
			DirVec = FVector(0, 1, 0);
			break;
		case 1:
			DirVec = FVector(0, -1, 0);
			break;
		case 2:
			DirVec = FVector(-1, 0, 0);
			break;
		case 3:
			DirVec = FVector(1, 0, 0);
			break;
		}

		for (auto& a : ActorsToTrack)
		{
			if (!IsValid(a)) { ActorsToRemove.Add(a); continue;}

			FVector BorderVector = (a->GetActorLocation() - GetActorLocation()).GetSafeNormal() * BorderWidth;
			BorderVector.Z = 0;
			FVector TrackingPointWithBorder = a->GetActorLocation() + BorderVector;

			float Dist = p.PlaneDot(TrackingPointWithBorder);
			FVector DistVec = (TrackingPointWithBorder - GetActorLocation());
			

			if (Dist >= DistOutside)	//Outside frustum
			{
				DistOutside = Dist;
				FurthestVec = DirVec * DistOutside;
				ClosestVec = FVector::ZeroVector;
			}
			else if (Dist < 0 && Dist > DistInside )	//inside frustum
			{
				if (FurthestVec == FVector::ZeroVector)
				{

					DistInside = Dist;
					ClosestVec = DirVec * DistInside;
				}
			}
			CurrentActor++;
		}
		sum -= FurthestVec*size;
		sum -= ClosestVec*size;
		CurrentPlane++;
	}

	for (auto& Actor : ActorsToRemove)
		ActorsToTrack.Remove(Actor);

	FHitResult OutHit; 
	LerpCameraLocation(GetActorLocation() + sum);

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
	if (!SceneView) { BWarn("Could not find scene view! "); return; }

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
	
	if (CameraZoom < SmallestSpringArmLength)
		CameraZoom = SmallestSpringArmLength;
	
	SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, CameraZoom, LerpAlpha);
}

void AGameCamera_B::SetSpringArmPitch()
{
	//Normalize length
	float Length = SpringArm->TargetArmLength;

	float NormalizedLength = (Length - SmallestSpringArmLength) / (LargestSpringArmLength - SmallestSpringArmLength);
	float PitchSetter = 1 - (NormalizedLength*NormalizedLength);
	if (PitchSetter > 1)
		PitchSetter = 1;
	//map normalization to the value
	float VariablePitch = (PitchSetter * (HighestRotAdd - LowestRotAdd)) + LowestRotAdd;

	//set value
	SpringArm->SetWorldRotation(FRotator(StartPitch + VariablePitch, SpringArm->GetComponentRotation().Yaw, SpringArm->GetComponentRotation().Roll));
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
}

void AGameCamera_B::OnTrackingBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APlayerCharacter_B::StaticClass()))	
		ActorsToTrack.Add(OtherActor);
}
