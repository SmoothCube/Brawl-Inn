// Fill out your copyright notice in the Description page of Project Settings.

#include "Useable_B.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "NiagaraComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "BrawlInn.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/MainGameMode_B.h"

AUseable_B::AUseable_B()
{

	NiagaraSystemComponent = CreateDefaultSubobject<UNiagaraComponent>("Particle System");
	NiagaraSystemComponent->SetupAttachment(RootComponent);

	Mesh->SetSimulatePhysics(false);

	PickupCapsule->SetRelativeLocation(FVector(0, 37, 0));
	PickupCapsule->SetCapsuleHalfHeight(60);
	PickupCapsule->SetCapsuleRadius(60);

	DrinkMesh = CreateDefaultSubobject<UStaticMeshComponent>("Drink Mesh");
	DrinkMesh->SetupAttachment(RootComponent);
	DrinkMesh->SetRelativeLocation(FVector(2.3f, 36.5f, 38));
	DrinkMesh->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.05f));
	DrinkMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWeight = 3.f;
}

void AUseable_B::BeginPlay()
{
	Super::BeginPlay();
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
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DetachFromActor(rules);
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

	DrinkMesh->DestroyComponent();
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

void AUseable_B::FellOutOfWorld(const UDamageType& dmgType)
{
	if (!GetWorld()->GetTimerManager().TimerExists(TH_DrinkHandle))
	{
		Super::FellOutOfWorld(dmgType);
	}
}
