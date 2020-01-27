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
#include "Player/PlayerCharacter_B.h"
#include "GameFramework/CharacterMovementComponent.h"


void AThrowable_B::PickedUp_Implementation(APlayerCharacter_B* Player)
{
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OwningPlayer = Player;
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
	APlayerCharacter_B* HitPlayer = Cast<APlayerCharacter_B>(OtherActor);
	if (HitPlayer && !HitPlayer->IsInvulnerable())
	{
		HitPlayer->GetCharacterMovement()->AddImpulse(GetVelocity() * ThrowHitStrength);
		BScreen("Overlapping with %s", *GetNameSafe(OtherActor));

		UGameplayStatics::ApplyDamage(HitPlayer, DamageAmount, OwningPlayer->GetController(), this, BP_DamageType);
	}
	Destroy();
}

void AThrowable_B::Use_Implementation()
{

	BWarn("Use Called!");

	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DetachFromActor(rules);
	Mesh->SetCollisionProfileName(FName("BlockAllDynamic"));
	Mesh->SetSimulatePhysics(true);
	PickupCapsule->SetCollisionProfileName(FName("Throwable-AfterThrow"));

	/// Throw with the help of AimAssist.
	FVector TargetLocation = OwningPlayer->GetActorForwardVector();   //Had a crash here, called from notify PlayerThrow_B. Added pointer check at top of function
	OwningPlayer->ThrowComponent->AimAssist(TargetLocation);
		
	Mesh->AddImpulse(TargetLocation * OwningPlayer->ThrowComponent->ImpulseSpeed, NAME_None, true);
	



}
