// Fill out your copyright notice in the Description page of Project Settings.

#include "Useable_B.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "NiagaraComponent.h"
#include "Engine/World.h"
#include "DestructibleComponent.h"
#include "TimerManager.h"

#include "BrawlInn.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/MainGameMode_B.h"
#include "Throwable_B.h"

AUseable_B::AUseable_B()
{
	NiagaraSystemComponent = CreateDefaultSubobject<UNiagaraComponent>("Particle System");
	NiagaraSystemComponent->SetRelativeLocation(FVector(0.f, -61.5f, 44.f));
	NiagaraSystemComponent->SetupAttachment(RootComponent);

	Mesh->SetSimulatePhysics(false);

	PickupCapsule->SetRelativeLocation(FVector(0, 37, 0));
	PickupCapsule->SetCapsuleHalfHeight(60);
	PickupCapsule->SetCapsuleRadius(60);
	PickupCapsule->SetCollisionProfileName("Powerup-CapsuleComponent");

	DrinkMesh = CreateDefaultSubobject<UStaticMeshComponent>("Drink Mesh");
	DrinkMesh->SetupAttachment(RootComponent);
	DrinkMesh->SetRelativeLocation(FVector(2.3f, -63.5, 38));
	DrinkMesh->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.05f));
	DrinkMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>("DestructibleComponent");
	DestructibleComponent->SetupAttachment(GetRootComponent());

	// Overriding variables
	PickupWeight = 3.f;
	HoldLocation = FVector(-20.920467, -3.708875, 7.292015);
}

UDestructibleComponent* AUseable_B::GetDestructibleComponent() const
{
	return DestructibleComponent;
}

void AUseable_B::BeginPlay()
{
	Super::BeginPlay();
	DestructibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AUseable_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AUseable_B::PickedUp_Implementation(ACharacter_B* Player)
{
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)

		OwningCharacter = Player;
}

void AUseable_B::Dropped_Implementation()
{
	const FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DetachFromActor(Rules);
	Mesh->SetCollisionProfileName(FName("BlockAllDynamic"));
	Mesh->SetSimulatePhysics(true);
}

void AUseable_B::Use_Implementation()
{
	if (Duration > 0)
		GetWorld()->GetTimerManager().SetTimer(TH_DrinkHandle, this, &AUseable_B::ResetBoost, Duration, false);

	if (DrinkSound)
	{
		UGameInstance_B* GameInstance = Cast<UGameInstance_B>(GetGameInstance());
		if (GameInstance)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DrinkSound, GetActorLocation(), 0.75 * GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume());
		}
	}

	PickupCapsule->DestroyComponent();
	NiagaraSystemComponent->DestroyComponent();

	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
		GameMode->RemoveCameraFocusPoint(this);

	OnFracture_Delegate.Broadcast();

	Execute_Dropped(this);
}

float AUseable_B::GetUseTime()
{
	return UseTime;
}

void AUseable_B::ResetBoost()
{
}

void AUseable_B::ThrowAway(FVector /*Direction*/)
{
	DestructibleComponent->SetCollisionProfileName("AfterFracture");
	DestructibleComponent->SetSimulatePhysics(true);
	Execute_Dropped(this);

	SetRootComponent(DestructibleComponent);
	Mesh->DestroyComponent();
	DrinkMesh->DestroyComponent();
	DestructibleComponent->SetSimulatePhysics(true);

	GetWorld()->GetTimerManager().SetTimer(TH_Despawn, this, &AUseable_B::BeginDespawn, GetWorld()->GetDeltaSeconds(), true, TimeBeforeDespawn);
	GetWorld()->GetTimerManager().SetTimer(TH_Destroy, this, &AUseable_B::StartDestroy, TimeBeforeDespawn + 0.1f, false);
}

void AUseable_B::StartDestroy()
{
	Destroy();
}

void AUseable_B::BeginDespawn()
{
	FVector Location = GetActorLocation();
	Location.Z -= DownValuePerTick;
	SetActorLocation(Location);
}

void AUseable_B::FellOutOfWorld(const UDamageType& dmgType)
{
	if (!GetWorld()->GetTimerManager().TimerExists(TH_DrinkHandle))
	{
		Super::FellOutOfWorld(dmgType);
	}
}
