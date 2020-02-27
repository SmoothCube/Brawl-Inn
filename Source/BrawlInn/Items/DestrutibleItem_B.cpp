// Fill out your copyright notice in the Description page of Project Settings.

#include "DestrutibleItem_B.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "BrawlInn.h"
#include "Characters/Character_B.h"
#include "Components/ThrowComponent_B.h"
#include "Components/DestructibleComponent_B.h"

ADestrutibleItem_B::ADestrutibleItem_B()
{
	Mesh->SetVisibility(false);
	Mesh->SetCollisionProfileName("BlockAllDynamicDestructible");
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(false);

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent_B>("Destructible");
	DestructibleComponent->SetupAttachment(GetRootComponent());
	DestructibleComponent->OnComponentFracture.AddDynamic(this, &ADestrutibleItem_B::OnFracture);
}

void ADestrutibleItem_B::BeginPlay()
{
	Super::BeginPlay();
}

void ADestrutibleItem_B::OnFracture(const FVector& HitPoint, const FVector& HitDirection)
{
	if (Mesh)
		Mesh->DestroyComponent();
	if (PickupCapsule)
		PickupCapsule->DestroyComponent();
	SetRootComponent(DestructibleComponent);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, [&]() {
		Destroy();
		}, 2.f, false);
}

void ADestrutibleItem_B::PickedUp_Implementation(ACharacter_B* Player)
{
	Mesh->SetVisibility(true);

	DestructibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DestructibleComponent->SetSimulatePhysics(false);

	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OwningCharacter = Player;
}

void ADestrutibleItem_B::Dropped_Implementation()
{
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DetachFromActor(rules);
	Mesh->SetCollisionProfileName(FName("BlockAllDynamic"));
	Mesh->SetSimulatePhysics(true);
	PickupCapsule->SetCollisionProfileName(FName("Throwable-AfterThrow"));
}

void ADestrutibleItem_B::Use_Implementation()
{
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
	DetachFromActor(rules);
	Mesh->SetCollisionProfileName(FName("BlockAllDynamicDestructible"));
	Mesh->SetSimulatePhysics(true);
	PickupCapsule->SetCollisionProfileName(FName("Throwable-AfterThrow"));

	/// Throw with the help of AimAssist.
	if (IsValid(OwningCharacter))
	{
		FVector TargetLocation = OwningCharacter->GetActorForwardVector();   //Had a crash here, called from notify PlayerThrow_B. Added pointer check at top of function
		OwningCharacter->ThrowComponent->AimAssist(TargetLocation);
		Mesh->AddImpulse(TargetLocation.GetSafeNormal() * OwningCharacter->ThrowComponent->ImpulseSpeed * 0.05f, NAME_None, true);
		Mesh->SetVisibility(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DestructibleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		DestructibleComponent->SetSimulatePhysics(true);
	}
	else
	{
		BError("No OwningCharacter for Throwable %s", *GetNameSafe(this))
	}
}

void ADestrutibleItem_B::OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	BLog("Hitting");
	ACharacter_B* HitPlayer = Cast<ACharacter_B>(OtherActor);
	if (HitPlayer && !HitPlayer->IsInvulnerable())
	{
		if (HitPlayer->HasShield())
		{
			HitPlayer->RemoveShield();
		}
		else
		{
			HitPlayer->GetCharacterMovement()->AddImpulse(GetVelocity() * ThrowHitStrength);
			HitPlayer->CheckFall(GetVelocity() * ThrowHitStrength);
			UGameplayStatics::ApplyDamage(HitPlayer, DamageAmount, OwningCharacter->GetController(), this, BP_DamageType);
		}
	}
	if (Mesh)
		Mesh->DestroyComponent();
	if (PickupCapsule)
		PickupCapsule->DestroyComponent();
	//Destroy();
}