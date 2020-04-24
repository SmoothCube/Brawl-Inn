// Fill out your copyright notice in the Description page of Project Settings.

#include "BounceActorSpawner_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "DestructibleComponent.h"
#include "TimerManager.h"
#include "Animation/AnimMontage.h"

#include "DrawDebugHelpers.h"

#include "BrawlInn.h"
#include "System/GameInstance_B.h"
#include "Components/MergeMeshComponent_B.h"
#include "Hazards/BounceActor/BounceActor_B.h"
#include "Characters/Player/RespawnPawn_B.h"

ABounceActorSpawner_B::ABounceActorSpawner_B()
{
	PrimaryActorTick.bCanEverTick = true;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlatformMesh");
	SetRootComponent(PlatformMesh);

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>("MainMesh");
	MainMesh->SetupAttachment(PlatformMesh);
	MainMesh->SetRelativeLocation({ 0.f,0.f,200.f });

	SmallCogMesh = CreateDefaultSubobject<UStaticMeshComponent>("SmallCogMesh");
	SmallCogMesh->SetupAttachment(MainMesh);
	SmallCogMesh->SetRelativeLocation({ 90.f, -180.f,-60.f });

	BigCogMesh = CreateDefaultSubobject<UStaticMeshComponent>("BigCogMesh");
	BigCogMesh->SetupAttachment(MainMesh);
	BigCogMesh->SetRelativeLocation({ -33.f, -180.f,-10.f });

	BarrelLowMesh = CreateDefaultSubobject<USkeletalMeshComponent>("BarrelLowMesh");
	BarrelLowMesh->SetupAttachment(MainMesh);
	BarrelLowMesh->SetRelativeLocation({ 0.f,0.f,-200.f });

	BarrelMidMesh = CreateDefaultSubobject<USkeletalMeshComponent>("BarrelMidMesh");
	BarrelMidMesh->SetupAttachment(BarrelLowMesh);

	BarrelTopMesh = CreateDefaultSubobject<USkeletalMeshComponent>("BarrelTopMesh");
	BarrelTopMesh->SetupAttachment(BarrelMidMesh);

	BarrelSpawnLocation = CreateDefaultSubobject<USceneComponent>("BarrelSpawnLocation");
	BarrelSpawnLocation->SetupAttachment(BarrelTopMesh);
	BarrelSpawnLocation->SetRelativeLocation({ 0.f,0.f,-800.f });

	OperatorNPCMesh = CreateDefaultSubobject<USkeletalMeshComponent>("OperatorNPCMesh");
	OperatorNPCMesh->SetupAttachment(MainMesh);
	OperatorNPCMesh->SetRelativeLocation({ -185.f, 0.f, -86.f });
	OperatorNPCMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	MergeMeshComponent = CreateDefaultSubobject<UMergeMeshComponent_B>("MergeComponent");

	EngineSoundComponent = CreateDefaultSubobject<UAudioComponent>("EngineSoundComponent");
	CogSoundComponent = CreateDefaultSubobject<UAudioComponent>("CogSoundComponent");
}

void ABounceActorSpawner_B::BeginPlay()
{
	Super::BeginPlay();
	if (MergeMeshComponent && MergeMeshComponent->bRandomizeOnBeginPlay)
	{
		MergeMeshComponent->CreateRandomMesh(OperatorNPCMesh);
		MergeMeshComponent->DestroyComponent();
	}
	
	if (CogSoundComponent)
	{
		CogSoundComponent->Stop();
	}
}

void ABounceActorSpawner_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(TH_ResetIsShooting);
}

void ABounceActorSpawner_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ShootTargets.IsValidIndex(0) && ShootTargets[0])
	{
		RotateCogs(DeltaTime);

		RotateMainCannon(DeltaTime, ShootTargets[0]->GetActorLocation());

		RotateBarrel(DeltaTime, ShootTargets[0]->GetActorLocation());

		FVector LaunchVel;
		UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), LaunchVel, BarrelSpawnLocation->GetComponentLocation(), ShootTargets[0]->GetActorLocation(), 0.0f, 0.5f);
		float DotProduct = FVector::DotProduct(LaunchVel.GetSafeNormal(), BarrelLowMesh->GetComponentRotation().Vector());
		
		DrawDebugLine(GetWorld(), BarrelSpawnLocation->GetComponentLocation(), BarrelSpawnLocation->GetComponentLocation() + LaunchVel.GetSafeNormal(), FColor::Blue, false, 0.5f);
		DrawDebugLine(GetWorld(), BarrelSpawnLocation->GetComponentLocation(), BarrelSpawnLocation->GetComponentLocation() + BarrelLowMesh->GetComponentRotation().Vector(),FColor::Red,false, 0.5f);
		if (FMath::IsNearlyEqual(DotProduct,1.f,0.005f))
		{
			if (bAnimationFinished)
			{
				bAnimationFinished = false;
				//SetIsShooting(false);
				PlayAnimationMontages();
			}
		}
	}
	else if (RotateTargets.IsValidIndex(0) && RotateTargets[0])
	{
		if (CogSoundComponent && !CogSoundComponent->IsPlaying())
			CogSoundComponent->Play();
		
		RotateCogs(DeltaTime);

		RotateMainCannon(DeltaTime, RotateTargets[0]->GetActorLocation());

		RotateBarrel(DeltaTime, RotateTargets[0]->GetActorLocation());
	}
	else
	{
		if (CogSoundComponent && CogSoundComponent->IsPlaying())
			CogSoundComponent->Stop();
	}
}

void ABounceActorSpawner_B::RotateBarrel(float DeltaTime, FVector TargetLocation)
{
	FVector LaunchVel = FVector::ZeroVector;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), LaunchVel, BarrelSpawnLocation->GetComponentLocation(), TargetLocation, 0.0f, 0.5f);
	
	LaunchVel = FVector::VectorPlaneProject(LaunchVel, MainMesh->GetRightVector());
	LaunchVel.Normalize();

	FRotator CurrentWorldRot = BarrelLowMesh->GetComponentRotation();
	
	float DotProduct = FVector::DotProduct(LaunchVel, CurrentWorldRot.Vector());

	float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
	Angle = (LaunchVel.Z > CurrentWorldRot.Vector().Z) ? Angle : -Angle;

	float newPitch = CurrentWorldRot.Pitch + Angle;

	//newPitch = FMath::Clamp(newPitch, LowestBarrelPitch, HighestBarrelPitch);

	BarrelLowMesh->SetWorldRotation(FMath::RInterpConstantTo(CurrentWorldRot, FRotator(newPitch, CurrentWorldRot.Yaw, CurrentWorldRot.Roll), DeltaTime, BarrelRotationSpeed));
}

void ABounceActorSpawner_B::RotateMainCannon(float DeltaTime, FVector TargetLocation)
{
	FVector RotationTarget = TargetLocation -  GetActorLocation();

	MainMesh->SetWorldRotation(FMath::RInterpConstantTo(MainMesh->GetRelativeRotation(), RotationTarget.ToOrientationRotator(), DeltaTime, CannonRotateSpeed));
}

void ABounceActorSpawner_B::RotateCogs(float DeltaTime)
{
	SmallCogMesh->SetRelativeRotation(FRotator(CurrentCogPitch, 0.f, 0.f));
	BigCogMesh->SetRelativeRotation(FRotator(20.f + CurrentCogPitch * -1.f, 0.f, 0.f));
	CurrentCogPitch += CogRotateSpeed * DeltaTime;
}

ABounceActor_B* ABounceActorSpawner_B::SpawnBounceActor()
{
	BWarn("Shooting Cannon!");
//	SetIsShooting(true);
	ABounceActor_B* NewBounceActor = GetWorld()->SpawnActor<ABounceActor_B>(ActorToSpawn, BarrelSpawnLocation->GetComponentLocation(), FRotator(90, 0, 0));
	if (!IsValid(NewBounceActor))
	{
		BError("Spawning Barrel Failed!");
		return nullptr;
	}

	FVector TargetLocation = FVector::ZeroVector;
	if (ShootTargets.IsValidIndex(0) && ShootTargets[0])
	{
		TargetLocation = ShootTargets[0]->GetActorLocation();
		ShootTargets[0]->SetupBarrel(NewBounceActor);
		ShootTargets.RemoveAt(0);
	}
	else
	{
		BError("SetupBarrel Failed!");
	}

	if (SpawnCue)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			SpawnCue,
			GetActorLocation(),
			1.f
		);
	}

	FVector LaunchVel = FVector::ZeroVector;
	UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), LaunchVel, NewBounceActor->GetActorLocation(), TargetLocation, 0.0f, 0.5f);
	NewBounceActor->GetMesh()->AddImpulse(LaunchVel, NAME_None, true);
	NewBounceActor->GetDestructibleComponent()->SetSimulatePhysics(true);

	return NewBounceActor;
}

void ABounceActorSpawner_B::AddRotateTarget(AActor* NewTarget)
{
	if (IsValid(NewTarget))
		RotateTargets.Add(NewTarget);
}

void ABounceActorSpawner_B::RemoveRotateTarget(AActor* NewTarget)
{
	if (IsValid(NewTarget))
		RotateTargets.Remove(NewTarget);
}

void ABounceActorSpawner_B::AddShootTarget(ARespawnPawn_B* NewTarget)
{
	if (IsValid(NewTarget))
		ShootTargets.Add(NewTarget);
	else
		BWarn("Adding shoot target failed!");
}

//bool ABounceActorSpawner_B::IsShooting()
//{
//	return bIsShooting;
//}
//
//void ABounceActorSpawner_B::SetIsShooting(bool Value)
//{
//	BWarn("Setting Is Shooting! %s", Value? TEXT("true") : TEXT("False"));
//	bIsShooting = Value;
//}

bool ABounceActorSpawner_B::IsAnimationFinished()
{

	return bAnimationFinished;
}

void ABounceActorSpawner_B::SetAnimationFinished(bool Value)
{
	BWarn("Setting Animation Finished! %s", Value ? TEXT("true") : TEXT("False"));
	bAnimationFinished = Value;
}

void ABounceActorSpawner_B::SetAnimationFinishedTimer()
{
	FTimerHandle TH_ResetAnimationTimer;
	GetWorld()->GetTimerManager().SetTimer(TH_ResetAnimationTimer, [&]()
		{
			bAnimationFinished = true;
		},
		1.f, 
		false);
}
