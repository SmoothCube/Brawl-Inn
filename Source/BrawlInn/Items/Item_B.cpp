// Fill out your copyright notice in the Description page of Project Settings.

#include "Item_B.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Sound/SoundCue.h"

#include "Characters/Character_B.h"
#include "System/GameInstance_B.h"

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

	OnFracture_Delegate.AddUObject(this, &AItem_B::OnItemFracture);
	PickupCapsule->OnComponentBeginOverlap.AddDynamic(this, &AItem_B::OnThrowOverlapBegin);
}

void AItem_B::FellOutOfWorld(const UDamageType& DmgType)
{
	OnFracture_Delegate.Broadcast();
	Super::FellOutOfWorld(DmgType);
}

UStaticMeshComponent* AItem_B::GetMesh() const
{
	return Mesh;
}

const FVector AItem_B::GetHoldLocation_Implementation()
{
	return HoldLocation;

}

const FRotator AItem_B::GetHoldRotation_Implementation()
{
	return HoldRotation;

}


bool AItem_B::IsHeld_Implementation() const
{
	return (IsValid(OwningCharacter));
}

bool AItem_B::CanBeHeld_Implementation() const
{
	return  !(bIsFractured || bIsThrown);
}

float AItem_B::GetThrowStrength_Implementation(EChargeLevel level) const
{
	switch (level)
	{
	case EChargeLevel::EChargeLevel1:
		return Charge1ThrowStrength;

	case EChargeLevel::EChargeLevel2:
		return Charge2ThrowStrength;

	case EChargeLevel::EChargeLevel3:
		return Charge3ThrowStrength;
	default:
		return 0;
	}
}

float AItem_B::GetPickupWeight_Implementation() const
{
	return PickupWeight;
}

float AItem_B::GetMovementSpeedWhenHeld_Implementation() const
{
	return MovementSpeedWhenHeld;
}

FOnFracture& AItem_B::OnFracture()
{
	return OnFracture_Delegate;
}

void AItem_B::OnItemFracture()
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PS_OnDestroy, GetActorLocation(), FRotator(90, 0, 0));

	if (DestroyedCue)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			DestroyedCue,
			GetActorLocation(),
			1.f
			);
	}

	bIsFractured = true;
}

void AItem_B::OnThrowOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == OwningCharacter || OtherActor->StaticClass() == this->StaticClass())
		return;

	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(this);
	if (Interface)
		if (!Interface->Execute_IsHeld(this)) return;

	OnHit(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AItem_B::OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}
