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
}

void AGameCamera_B::BeginPlay()
{
	Super::BeginPlay();

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

	//Cleanup in case some components dont get removed properly
	TArray<USceneComponent*> CompsToRemove;

	for (const auto& Comp : ComponentsToTrack)
	{
		if (!IsValid(Comp))
		{
			BWarn("Invalid component in camera!");
			CompsToRemove.Add(Comp);
			continue;
		}

		FVector PlayerMeshLocation = Comp->GetComponentLocation();
		sum += PlayerMeshLocation;
		ActivePlayers++;

		float distance = FVector::Dist(PlayerMeshLocation, GetActorLocation());
		if (distance > distanceToFurthestPlayer)
			distanceToFurthestPlayer = distance;

	}
	for (auto& Comp : CompsToRemove)
		ComponentsToTrack.Remove(Comp);

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

	for (int i = 0; i < ComponentsToTrack.Num(); i++)
	{
		for (int j = i + 1; j < ComponentsToTrack.Num(); j++)
		{
			if (ComponentsToTrack.IsValidIndex(i) && ComponentsToTrack.IsValidIndex(j))
			{
				float Temp = FMath::Abs((ComponentsToTrack[i]->GetComponentLocation() - ComponentsToTrack[j]->GetComponentLocation()).X);
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

void AGameCamera_B::OnTrackingBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//The right component doesnt neccesarily run on overlap end
	//GetComponents returns UActorComponent, which inherits from USceneComponent. Cast should always be safe. 
	for(auto& comp : OtherActor->GetComponents())
		ComponentsToTrack.Remove(Cast<USceneComponent>(comp));

	APlayerCharacter_B* Player = Cast<APlayerCharacter_B>(OtherActor);
	if (Player)
	{
		//Checks to see if the player is still overlapping. Same method as in DragArea
		TArray<UPrimitiveComponent*> OverlappingComponents;
		TrackingBox->GetOverlappingComponents(OverlappingComponents);
		for (auto& comp : OverlappingComponents)
		{
			AActor* Actor = comp->GetOwner();
			
			APlayerCharacter_B* OtherPlayer = Cast<APlayerCharacter_B>(Actor);

			if (OtherPlayer != nullptr)
			{
				//Checks to see if player still exist in the array
				bool bActorIsAdded = false;
				for (auto& ExistingComponent : ComponentsToTrack)
				{
					if (Actor == ExistingComponent->GetOwner())
					{
						bActorIsAdded = true;
						break;
					}
				}
				if (bActorIsAdded)
				{
					continue;
				}
				
				ComponentsToTrack.Add(comp);
			}
		}
	}

	//Logging the actors for debug purpouses
	for (auto& c : ComponentsToTrack)
	{
		BWarn("Component in camera: %s, Owner: %s", *GetNameSafe(c), *GetNameSafe(c->GetOwner()))
	}

}

void AGameCamera_B::OnTrackingBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor->IsA(APlayerCharacter_B::StaticClass()))	
		ComponentsToTrack.Add(OtherComp);
}
