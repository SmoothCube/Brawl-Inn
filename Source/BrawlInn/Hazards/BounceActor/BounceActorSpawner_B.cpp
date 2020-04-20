// Fill out your copyright notice in the Description page of Project Settings.

#include "BounceActorSpawner_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "DestructibleComponent.h"

#include "BrawlInn.h"
#include "System/GameInstance_B.h"
#include "Components/MergeMeshComponent_B.h"
#include "Hazards/BounceActor/BounceActor_B.h"

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

	CannonBarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>("CannonBarrelMesh");
	CannonBarrelMesh->SetupAttachment(MainMesh);
	CannonBarrelMesh->SetRelativeLocation({ 0.f,0.f,-200.f });

	BarrelSpawnLocation = CreateDefaultSubobject<USceneComponent>("BarrelSpawnLocation");
	BarrelSpawnLocation->SetupAttachment(CannonBarrelMesh);
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

	float Volume = 1.f;

	UGameInstance_B* GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance)
	{
		Volume *= GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume();
	}

	if (EngineSoundComponent)
		EngineSoundComponent->SetVolumeMultiplier(Volume);
	if (CogSoundComponent)
	{
		CogSoundComponent->SetVolumeMultiplier(Volume);
		CogSoundComponent->Stop();
	}
}

void ABounceActorSpawner_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RotateTargets.IsValidIndex(0) && RotateTargets[0])
	{
		if (CogSoundComponent && !CogSoundComponent->IsPlaying())
			CogSoundComponent->Play();
		SmallCogMesh->SetRelativeRotation(FRotator(CurrentCogPitch, 0.f, 0.f));
		BigCogMesh->SetRelativeRotation(FRotator(CurrentCogPitch * -1.f, 0.f, 0.f));
		CurrentCogPitch += CogRotateSpeed * DeltaTime;

		FVector RotationTarget = GetActorLocation() - RotateTargets[0]->GetActorLocation();

		FMath::RInterpConstantTo(MainMesh->GetRelativeRotation(), RotationTarget.ToOrientationRotator(), DeltaTime, CannonRotateSpeed);
		MainMesh->SetRelativeRotation(FMath::RInterpConstantTo(MainMesh->GetRelativeRotation(), RotationTarget.ToOrientationRotator(), DeltaTime, CannonRotateSpeed));

		FVector LaunchVel = FVector::ZeroVector;
		UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), LaunchVel, BarrelSpawnLocation->GetComponentLocation(), RotateTargets[0]->GetActorLocation(), 0.0f, 0.5f);

		LaunchVel = FVector::VectorPlaneProject(LaunchVel, MainMesh->GetRightVector());
		FRotator CurrentRot = CannonBarrelMesh->GetRelativeRotation();
		float DotProduct = FVector::DotProduct(LaunchVel.GetSafeNormal(), CurrentRot.Vector());

		CannonBarrelMesh->SetRelativeRotation(FMath::RInterpConstantTo(CurrentRot, FRotator(CurrentRot.Pitch - (DotProduct), CurrentRot.Yaw, CurrentRot.Roll), DeltaTime, CannonRotateSpeed));
	}
	else
	{
		if (CogSoundComponent && CogSoundComponent->IsPlaying())
			CogSoundComponent->Stop();
	}
}

ABounceActor_B* ABounceActorSpawner_B::SpawnBounceActor(FVector TargetLocation)
{
	ABounceActor_B* NewBounceActor = GetWorld()->SpawnActor<ABounceActor_B>(ActorToSpawn, BarrelSpawnLocation->GetComponentLocation(), FRotator(90, 0, 0));
	if (!IsValid(NewBounceActor))
		return nullptr;

	if (SpawnCue)
	{
		float Volume = 1.f;
		UGameInstance_B* GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (GameInstance)
		{
			Volume *= GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume();
		}
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			SpawnCue,
			GetActorLocation(),
			Volume
		);
	}

	NewBounceActor->SetActorRotation(FRotator(0, 50, 0));
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
