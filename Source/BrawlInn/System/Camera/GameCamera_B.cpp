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
#include "Engine/TargetPoint.h"
#include "TimerManager.h"

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
	if (Actors.IsValidIndex(0))
		TrackingBox = Cast<ATriggerBox>(Actors[0]);
	if (TrackingBox)
	{
		TrackingBox->GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &AGameCamera_B::OnTrackingBoxEndOverlap);
		TrackingBox->GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AGameCamera_B::OnTrackingBoxBeginOverlap);
	}

	//Caches the camera rotation angle
	StartPitch = SpringArm->GetComponentRotation().Pitch;
	//	SetActorTickEnabled(false);
}

void AGameCamera_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//finds the camera frustum
	FMatrix Proj, View, ViewProj;

	FMinimalViewInfo ViewInfo;
	Camera->GetCameraView(GetWorld()->GetDeltaSeconds(), ViewInfo);
	UGameplayStatics::GetViewProjectionMatrix(ViewInfo, View, Proj, ViewProj);
	FConvexVolume v;
	GetViewFrustumBounds(v, ViewProj, true);

	UpdateCameraPosition(v);
	SetSpringArmPitch();
	SetSpringArmLength(v);

}

void AGameCamera_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

}

void AGameCamera_B::UpdateCameraPosition(FConvexVolume& scene)
{
	FVector Sum = FVector::ZeroVector;

	TArray<AActor*> ActorsToRemove;
	for (int i = 0; i < scene.Planes.Num() - 1; i++)
	{
		auto& p = scene.Planes[i];
		float DistOutside = 0;
		float DistInside = -10000000;
		FVector DistVec = FVector::ZeroVector;

		FVector DirVec = FVector::ZeroVector;
		float BorderWidth = 0.f;
		switch (i)
		{
		case 0:
			BorderWidth = RightBorderWidth;
			DirVec = Camera->GetRightVector();
			break;
		case 1:
			BorderWidth = LeftBorderWidth;
			DirVec = -Camera->GetRightVector();
			break;
		case 2:
			BorderWidth = DownBorderWidth;
			DirVec = -Camera->GetUpVector();
			break;
		case 3:
			BorderWidth = UpBorderWidth;
			DirVec = Camera->GetUpVector();
			break;
		}
		for (auto& a : ActorsToTrack)
		{
			if (!IsValid(a)) { ActorsToRemove.Add(a); continue; }

			//find player position with border
			FVector BorderVector = DirVec.GetSafeNormal() * BorderWidth;
			FVector TrackingPointWithBorder = a->GetActorLocation() + BorderVector;

			float Dist = p.PlaneDot(TrackingPointWithBorder);

			if (Dist >= DistOutside)	//Outside frustum
			{
				DistOutside = Dist;
				DistVec = DirVec * DistOutside;
			}
			else if (Dist < 0 && Dist > DistInside)	//inside frustum
			{
				if (DistOutside == 0)
				{
					DistInside = Dist;
					DistVec = DirVec * DistInside;
				}
			}
		}
		Sum -= DistVec * CameraMoveSpeed;
	}

	for (auto& Actor : ActorsToRemove)
		ActorsToTrack.Remove(Actor);

	LerpCameraLocation(GetActorLocation() + Sum);

}

void AGameCamera_B::LerpCameraLocation(FVector LerpLoc)
{
	SetActorLocation(FMath::Lerp(GetActorLocation(), LerpLoc, LerpAlpha));
}

void AGameCamera_B::SetSpringArmLength(FConvexVolume& scene)
{
	float FurthestDist = -1000000;
	for (int i = 0; i < scene.Planes.Num() - 1; i++)
	{
		FVector DirVec = FVector::ZeroVector;
		float BorderWidth = 0.f;
		switch (i)
		{
		case 0:
			BorderWidth = LeftBorderWidth;
			DirVec = Camera->GetRightVector();
			break;
		case 1:
			BorderWidth = RightBorderWidth;
			DirVec = -Camera->GetRightVector();
			break;
		case 2:
			BorderWidth = UpBorderWidth;
			DirVec = -Camera->GetUpVector();
			break;
		case 3:
			BorderWidth = DownBorderWidth;
			DirVec = Camera->GetUpVector();
			break;
		}

		FVector BorderVector = DirVec.GetSafeNormal() * -BorderWidth;

		for (auto& a : ActorsToTrack)
		{
			FVector TrackingPointWithBorder = a->GetActorLocation() + BorderVector;

			float Dist = scene.Planes[i].PlaneDot(TrackingPointWithBorder);
			if (Dist > 0 && Dist > FurthestDist) //outside frustum
				FurthestDist = Dist;
			else if (Dist < 0 && Dist > FurthestDist && FurthestDist <= 0) //inside and nothing on outside so far 
				FurthestDist = Dist;
		}
	}

	CameraZoom += FurthestDist * CameraZoomSpeed;

	CameraZoom = FMath::Clamp(CameraZoom, SmallestSpringArmLength, LargestSpringArmLength);

	SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, CameraZoom, LerpAlpha);
}

void AGameCamera_B::SetSpringArmPitch()
{
	//Normalize length
	float Length = SpringArm->TargetArmLength;

	float NormalizedLength = (Length - SmallestSpringArmLength) / (LargestSpringArmLength - SmallestSpringArmLength);
	float PitchSetter = -(NormalizedLength* NormalizedLength) + 2*NormalizedLength;
	PitchSetter = FMath::Clamp(PitchSetter, 0.f, 1.f); //PitchSetter should never be anything else, but keep this just to be sure

	//map normalization to the value
	float VariablePitch = (PitchSetter * (LowestRotAdd - HighestRotAdd)) + HighestRotAdd;

	//set value
	SpringArm->SetWorldRotation(FRotator(StartPitch + VariablePitch, SpringArm->GetComponentRotation().Yaw, SpringArm->GetComponentRotation().Roll));
}

void AGameCamera_B::EndFocus(AActor* OtherActor)
{
	if (!IsValid(OtherActor))
		BWarn("No Other Actor!");
	ActorsToTrack.Remove(OtherActor);
	ATargetPoint* Target = Cast<ATargetPoint>(OtherActor);
	if (Target)
		Target->Destroy();
}

void AGameCamera_B::OnTrackingBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	FTimerHandle UniqueHandle;
	//end focus for player and spawn a targetpoint and focus that 
	EndFocus(OtherActor);

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
		else
		{
			AActor* Target = GetWorld()->SpawnActor<ATargetPoint>(ATargetPoint::StaticClass(), OtherActor->GetActorTransform());
			if (IsValid(Target))
			{
				ActorsToTrack.Add(Target);

				FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &AGameCamera_B::EndFocus, Target);
				GetWorldTimerManager().SetTimer(UniqueHandle, RespawnDelegate, EndFocusTime, false);
			}
		}
	}
}
void AGameCamera_B::OnTrackingBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APlayerCharacter_B::StaticClass()))
		ActorsToTrack.Add(OtherActor);
}
