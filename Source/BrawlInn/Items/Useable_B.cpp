// Fill out your copyright notice in the Description page of Project Settings.


#include "Useable_B.h"
#include "BrawlInn.h"
#include "Components/StaticMeshComponent.h"

AUseable_B::AUseable_B()
{
	Mesh->SetSimulatePhysics(false);
}

void AUseable_B::PickedUp_Implementation(APlayerCharacter_B* Player)
{
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OwningPlayer = Player;
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
	BLog("Using drink");
	Execute_Dropped(this);
}

void AUseable_B::OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

