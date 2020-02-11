// Fill out your copyright notice in the Description page of Project Settings.

#include "Throwable_B.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

#include "BrawlInn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/HoldComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "Characters/Character_B.h"
#include "GameFramework/CharacterMovementComponent.h"

void AThrowable_B::PickedUp_Implementation(ACharacter_B* Player)
{
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OwningCharacter = Player;
}

void AThrowable_B::Dropped_Implementation()
{
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DetachFromActor(rules);
	Mesh->SetCollisionProfileName(FName("BlockAllDynamic"));
	Mesh->SetSimulatePhysics(true);
	PickupCapsule->SetCollisionProfileName(FName("Throwable-AfterThrow"));
}

void AThrowable_B::OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter_B* HitPlayer = Cast<ACharacter_B>(OtherActor);
	if (HitPlayer && !HitPlayer->IsInvulnerable())
	{
		if (HitPlayer->bHasShield)
		{
			HitPlayer->RemoveShield();
		}
		else
		{
			HitPlayer->GetCharacterMovement()->AddImpulse(GetVelocity() * ThrowHitStrength);

			UGameplayStatics::ApplyDamage(HitPlayer, DamageAmount, OwningCharacter->GetController(), this, BP_DamageType);
		}
	}
	Destroy();
}

void AThrowable_B::Use_Implementation()
{
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DetachFromActor(rules);
	Mesh->SetCollisionProfileName(FName("BlockAllDynamic"));
	Mesh->SetSimulatePhysics(true);
	PickupCapsule->SetCollisionProfileName(FName("Throwable-AfterThrow"));

	/// Throw with the help of AimAssist.
	if (IsValid(OwningCharacter))
	{
		FVector TargetLocation = OwningCharacter->GetActorForwardVector();   //Had a crash here, called from notify PlayerThrow_B. Added pointer check at top of function
		OwningCharacter->ThrowComponent->AimAssist(TargetLocation);
		Mesh->AddImpulse(TargetLocation.GetSafeNormal() * OwningCharacter->ThrowComponent->ImpulseSpeed, NAME_None, true);
	}
	else
	{
		BError("No OwningCharacter for Throwable %s", *GetNameSafe(this))
	}
	OwningCharacter = nullptr;

}
