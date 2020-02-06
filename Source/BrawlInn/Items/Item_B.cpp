// Fill out your copyright notice in the Description page of Project Settings.

#include "Item_B.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

#include "Characters/Player/PlayerCharacter_B.h"
#include "System/GameInstance_B.h"
#include "Components/HoldComponent_B.h"
AItem_B::AItem_B()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetCollisionProfileName("BlockAllDynamic");
	Mesh->SetSimulatePhysics(true);
	RootComponent = Mesh;

	PickupCapsule = CreateDefaultSubobject<UCapsuleComponent>("Sphere");
	PickupCapsule->SetCollisionProfileName("Throwable-Trigger");
	PickupCapsule->SetupAttachment(Mesh);
}

bool AItem_B::IsHeld_Implementation() const
{
	if (OwningPlayer)
		return true;
	return false;
}

void AItem_B::Use_Implementation()
{

}
void AItem_B::BeginPlay()
{
	Super::BeginPlay();
	PickupCapsule->OnComponentBeginOverlap.AddDynamic(this, &AItem_B::OnThrowOverlapBegin);
}

void AItem_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PS_OnDestroy, GetActorLocation());
		if (OwningPlayer)
		{
			if (OwningPlayer->HoldComponent)
			{
				OwningPlayer->HoldComponent->SetHoldingItem(nullptr);
				OwningPlayer->SetState(EState::EWalking);
			}
			OwningPlayer = nullptr;
		}

		if (DestroyedCue)
		{
			float volume = 1.f;
			UGameInstance_B* GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
			if (GameInstance)
			{
				volume *= GameInstance->MasterVolume * GameInstance->SfxVolume;
			}
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				DestroyedCue,
				GetActorLocation(),
				volume
			);
		}
	}
}

void AItem_B::OnThrowOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(this);
	if (Interface)
	{
		if (!Interface->Execute_IsHeld(this)) return;
	}
	if (!IsHeld_Implementation() || OtherActor == OwningPlayer || OtherActor->StaticClass() == this->StaticClass())
		return;

	OnHit(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AItem_B::OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}
