// Fill out your copyright notice in the Description page of Project Settings.


#include "DestrutibleItem_B.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
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
	Mesh->DestroyComponent();
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
	//Mesh->SetCollisionProfileName(FName("BlockAllDynamic"));
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
		Mesh->AddImpulse(TargetLocation.GetSafeNormal() * OwningCharacter->ThrowComponent->ImpulseSpeed, NAME_None, true);
		Mesh->SetVisibility(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DestructibleComponent->Activate(true);
		DestructibleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DestructibleComponent->SetSimulatePhysics(true);
	}
	else
	{
		BError("No OwningCharacter for Throwable %s", *GetNameSafe(this))
	}
}
