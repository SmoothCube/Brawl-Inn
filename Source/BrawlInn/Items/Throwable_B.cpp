// Fill out your copyright notice in the Description page of Project Settings.


#include "Throwable_B.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "BrawlInn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Player/PlayerCharacter_B.h"

AThrowable_B::AThrowable_B()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
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
	APlayerCharacter_B* HitPlayer = Cast<APlayerCharacter_B>(OtherActor);
	if (HitPlayer)
	{
		HitPlayer->GetCharacterMovement()->AddImpulse(GetVelocity()* ThrowHitStrength);
		BScreen("Overlapping with %s", *GetNameSafe(OtherActor));

		UGameplayStatics::ApplyDamage(HitPlayer,DamageAmount, OwningPlayer->GetController(),this, BP_DamageType);
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
