// Fill out your copyright notice in the Description page of Project Settings.

#include "BounceActorSpawner_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Sound/SoundCue.h"
#include "DestructibleComponent.h"

#include "System/GameInstance_B.h"
#include "Hazards/BounceActor/BounceActor_B.h"

ABounceActorSpawner_B::ABounceActorSpawner_B()
{
	PrimaryActorTick.bCanEverTick = false;
}

ABounceActor_B* ABounceActorSpawner_B::SpawnBounceActor(FVector TargetLocation)
{
	ABounceActor_B* NewBounceActor = GetWorld()->SpawnActor<ABounceActor_B>(ActorToSpawn, GetActorLocation(), FRotator(90, 0, 0));
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
