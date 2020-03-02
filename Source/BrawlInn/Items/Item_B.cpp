// Fill out your copyright notice in the Description page of Project Settings.

#include "Item_B.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

#include "BrawlInn.h"
#include "Characters/Character_B.h"
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

void AItem_B::BeginPlay()
{
	Super::BeginPlay();

	OnFracture.AddUObject(this, &AItem_B::OnItemFracture);
	PickupCapsule->OnComponentBeginOverlap.AddDynamic(this, &AItem_B::OnThrowOverlapBegin);
}

bool AItem_B::IsHeld_Implementation() const
{
	return (IsValid(OwningCharacter));
}

bool AItem_B::CanBeHeld_Implementation() const
{
	return bCanBeHeld;
}

void AItem_B::OnItemFracture()
{
	BLog("Destroy");
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PS_OnDestroy, GetActorLocation(), FRotator(90, 0, 0));

	if (DestroyedCue)
	{
		float volume = 1.f;
		UGameInstance_B* GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (GameInstance)
		{
			volume *= GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume();
		}
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			DestroyedCue,
			GetActorLocation(),
			volume
		);
	}

	bCanBeHeld = false;
}

void AItem_B::OnThrowOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == OwningCharacter || OtherActor->StaticClass() == this->StaticClass())
		return;
	
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(this);
	if (Interface)
	{
		if (!Interface->Execute_IsHeld(this)) return;
	}

	OnHit(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AItem_B::OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}
