// Fill out your copyright notice in the Description page of Project Settings.


#include "Throwable_B.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "BrawlInn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/HoldComponent_B.h"
#include "Player/PlayerCharacter_B.h"

AThrowable_B::AThrowable_B()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;

	PickupCapsule = CreateDefaultSubobject<UCapsuleComponent>("Sphere");
	PickupCapsule->SetupAttachment(Mesh);
}

void AThrowable_B::BeginPlay()
{
	Super::BeginPlay();
	PickupCapsule->OnComponentBeginOverlap.AddDynamic(this, &AThrowable_B::OnThrowOverlapBegin);
}

void AThrowable_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ParticleSystem, GetActorLocation());
		if (OwningPlayer)
		{
			OwningPlayer->HoldComponent->SetHoldingItem(nullptr);
			OwningPlayer = nullptr;
		}
	}
}

void AThrowable_B::OnThrowOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsHeld() || OtherActor == OwningPlayer || OtherActor->StaticClass() == this->StaticClass())
		return;
	APlayerCharacter_B* HitPlayer = Cast<APlayerCharacter_B>(OtherActor);
	if (HitPlayer)
	{
		HitPlayer->GetCharacterMovement()->AddImpulse(GetVelocity() * ThrowHitStrength);
		BScreen("Overlapping with %s", *GetNameSafe(OtherActor));

		UGameplayStatics::ApplyDamage(HitPlayer, DamageAmount, OwningPlayer->GetController(), this, BP_DamageType);
	}
	Destroy();
}

void AThrowable_B::PickedUp(APlayerCharacter_B* Player)
{
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OwningPlayer = Player;
}

void AThrowable_B::Dropped()
{
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DetachFromActor(rules);
	Mesh->SetCollisionProfileName(FName("BlockAllDynamic"));
	Mesh->SetSimulatePhysics(true);
	PickupCapsule->SetCollisionProfileName(FName("Throwable-AfterThrow"));
}

bool AThrowable_B::IsHeld() const
{
	if (OwningPlayer)
		return true;
	return false;
}