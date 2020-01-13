// Fill out your copyright notice in the Description page of Project Settings.


#include "Throwable_B.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

#include "BrawlInn.h"
#include "Player/PlayerCharacter_B.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "DestructibleComponent.h"

AThrowable_B::AThrowable_B()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UDestructibleComponent>("Mesh");
	RootComponent = Mesh;

	PickupSphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	PickupSphere->SetupAttachment(Mesh);
	PickupSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
}

void AThrowable_B::BeginPlay()
{

	Super::BeginPlay();

	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &AThrowable_B::OnThrowOverlapBegin);

}

void AThrowable_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		BWarn("Spawning particles from %s system.", *GetNameSafe(ParticleSystem));
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ParticleSystem, GetActorLocation());
	}
}

void AThrowable_B::OnThrowOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsHeld() || OtherActor == OwningPlayer || OtherActor->StaticClass() == this->StaticClass())
		return;

	Destroy();
	
	BScreen("Overlapping with %s", *GetNameSafe(OtherActor));
}

void AThrowable_B::PickedUp(APlayerCharacter_B* Owner)
{
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OwningPlayer = Owner;

}

void AThrowable_B::Dropped()
{
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
	PickupSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
}

bool AThrowable_B::IsHeld() const
{
	if (OwningPlayer)
		return true;
	return false;
}




