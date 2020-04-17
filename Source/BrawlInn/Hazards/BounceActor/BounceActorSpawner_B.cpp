// Fill out your copyright notice in the Description page of Project Settings.

#include "BounceActorSpawner_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Sound/SoundCue.h"
#include "DestructibleComponent.h"

#include "BrawlInn.h"
#include "System/GameInstance_B.h"
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
	SmallCogMesh->SetRelativeLocation({ 180.f, 90.f,-60.f });

	BigCogMesh = CreateDefaultSubobject<UStaticMeshComponent>("BigCogMesh");
	BigCogMesh->SetupAttachment(MainMesh);
	BigCogMesh->SetRelativeLocation({ 180.f, -32.f,-11.f });

	CannonBarrelMesh = CreateDefaultSubobject<UStaticMeshComponent>("CannonBarrelMesh");
	CannonBarrelMesh->SetupAttachment(MainMesh);
	CannonBarrelMesh->SetRelativeLocation({0.f,0.f,-200.f });

	BarrelSpawnLocation = CreateDefaultSubobject<USceneComponent>("BarrelSpawnLocation");
	BarrelSpawnLocation->SetupAttachment(CannonBarrelMesh);
	BarrelSpawnLocation->SetRelativeLocation({ 0.f,0.f,-800.f });

}

void ABounceActorSpawner_B::Tick(float DeltaTime)
{
	SmallCogMesh->AddRelativeRotation(FRotator(0.f, 0.f, CogRotateSpeed * DeltaTime));
	BigCogMesh->AddRelativeRotation(FRotator(0.f, 0.f, CogRotateSpeed * DeltaTime * -1));

	FRotator RotationTarget = GetActorRotation();
	if (RotateTargets.IsValidIndex(0) && RotateTargets[0])
	{
		FVector RotationTarget =  GetActorLocation() - RotateTargets[0]->GetActorLocation();
		RotationTarget;
		FMath::RInterpConstantTo(MainMesh->GetRelativeRotation(), RotationTarget.ToOrientationRotator(), DeltaTime, CannonRotateSpeed);
		MainMesh->SetRelativeRotation(FMath::RInterpConstantTo(MainMesh->GetRelativeRotation(), RotationTarget.ToOrientationRotator(), DeltaTime, CannonRotateSpeed));

		FVector LaunchVel = FVector::ZeroVector;
		UGameplayStatics::SuggestProjectileVelocity_CustomArc(GetWorld(), LaunchVel, BarrelSpawnLocation->GetComponentLocation(),RotateTargets[0]->GetActorLocation(), 0.0f, 0.5f);

		LaunchVel = FVector::VectorPlaneProject(LaunchVel, MainMesh->GetForwardVector());
		FRotator CurrentRot = CannonBarrelMesh->GetRelativeRotation();
		float AngleInDegrees = FMath::Acos(FVector::DotProduct(LaunchVel, CurrentRot.Vector()));

		BWarn("Angle: %f", AngleInDegrees);
		CurrentRot += FRotator(-90.f, 0.f, 0.f);
		//FMath::RInterpConstantTo(MainMesh->GetRelativeRotation(), FRotator(CurrentRot.Pitch, CurrentRot.Yaw, AngleInDegrees), CannonRotateSpeed);

		//CannonBarrelMesh->AddRelativeRotation(FRotator(CurrentRot.Pitch, CurrentRot.Yaw, AngleInDegrees));
		//CannonBarrelMesh->SetRelativeRotation(FMath::RInterpConstantTo(CurrentRot, FRotator(CurrentRot.Pitch, CurrentRot.Yaw, CurrentRot.Roll + AngleInDegrees), DeltaTime, CannonRotateSpeed));

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
