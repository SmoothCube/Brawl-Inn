// Fill out your copyright notice in the Description page of Project Settings.

#include "Throwable_B.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DestructibleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "BrawlInn.h"
#include "Characters/Character_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/ThrowComponent_B.h"
#include "System/DataTable_B.h"

AThrowable_B::AThrowable_B()
{
	Mesh->SetVisibility(false);
	Mesh->SetCollisionProfileName("BlockAllDynamicDestructible");
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>("Destructible");
	DestructibleComponent->SetupAttachment(GetRootComponent());
	DestructibleComponent->SetSimulatePhysics(true);
}

void AThrowable_B::BeginPlay()
{
	Super::BeginPlay();
	DestructibleComponent->OnComponentFracture.Clear();
	DestructibleComponent->OnComponentFracture.AddDynamic(this, &AThrowable_B::OnComponentFracture);

	UDataTable_B* DataTable = NewObject<UDataTable_B>();
	DataTable->LoadCSVFile(FScoreTable::StaticStruct(), "DefaultScoreValues.csv");
	ScoreAmount = DataTable->GetRow<FScoreTable>("Throwable")->Value;
}

void AThrowable_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestructibleComponent->OnComponentFracture.Clear();

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	GetWorld()->GetTimerManager().ClearTimer(TH_Despawn);
}

void AThrowable_B::PickedUp_Implementation(ACharacter_B* Player)
{
	Mesh->SetVisibility(true);

	DestructibleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DestructibleComponent->SetSimulatePhysics(false);

	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OwningCharacter = Player;
}

void AThrowable_B::Dropped_Implementation()
{
	const FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DetachFromActor(Rules);
	Mesh->SetCollisionProfileName(FName("BlockAllDynamicDestructible"));
	Mesh->SetSimulatePhysics(true);
	PickupCapsule->SetCollisionProfileName(FName("Throwable-AfterThrow"));

	Mesh->SetVisibility(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DestructibleComponent->SetCollisionProfileName("Destructible");
	DestructibleComponent->SetSimulatePhysics(true);
}

void AThrowable_B::Use_Implementation()
{
	const FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
	DetachFromActor(Rules);
	Mesh->SetCollisionProfileName(FName("BlockAllDynamicDestructible"));
	Mesh->SetSimulatePhysics(true);
	PickupCapsule->SetCollisionProfileName(FName("Throwable-AfterThrow"));

	/// Throw with the help of AimAssist.
	if (IsValid(OwningCharacter))
	{
		FVector TargetLocation = OwningCharacter->GetActorForwardVector();   //Had a crash here, called from notify PlayerThrow_B. Added pointer check at top of function
		OwningCharacter->ThrowComponent->AimAssist(TargetLocation);
		float ImpulseStrength = 0.f;
		IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(this);
		if (Interface)
		{
			ImpulseStrength = Interface->Execute_GetThrowStrength(this);
		}
		Mesh->AddImpulse(TargetLocation.GetSafeNormal() * ImpulseStrength, NAME_None, true);
		Mesh->SetVisibility(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DestructibleComponent->SetCollisionProfileName("Destructible");
		DestructibleComponent->SetSimulatePhysics(true);
		DestructibleComponent->SetPhysicsLinearVelocity(FVector(0.f, 0.f, 0.f));
		DestructibleComponent->SetPhysicsAngularVelocityInDegrees(FVector(0.f, 0.f, 0.f));
	}
	else
	{
		BError("No OwningCharacter for Throwable %s", *GetNameSafe(this))
	}
	bIsThrown = true;
}

void AThrowable_B::OnComponentFracture(const FVector& HitPoint, const FVector& HitDirection)
{
	if (Mesh)
		Mesh->DestroyComponent();
	if (PickupCapsule)
		PickupCapsule->DestroyComponent();
	SetRootComponent(DestructibleComponent);

	DestructibleComponent->SetCollisionProfileName("AfterFracture");
	DestructibleComponent->SetGenerateOverlapEvents(false);

	OnFracture_Delegate.Broadcast();

	GetWorld()->GetTimerManager().SetTimer(TH_Despawn, this, &AThrowable_B::BeginDespawn, GetWorld()->GetDeltaSeconds(), true, TimeBeforeDespawn);
	GetWorld()->GetTimerManager().SetTimer(TH_Destroy, this, &AThrowable_B::StartDestroy, TimeBeforeDespawn + 0.1f, false);
}

void AThrowable_B::StartDestroy()
{
	Destroy();
}

void AThrowable_B::BeginDespawn()
{
	FVector Location = GetActorLocation();
	Location.Z -= DownValuePerTick;
	SetActorLocation(Location);
}

void AThrowable_B::OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	BWarn("Throwable Hit"); //TODO remove
	if (OtherComp->IsA(UHoldComponent_B::StaticClass()))
		return;

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
			UGameplayStatics::ApplyDamage(HitPlayer, ScoreAmount, OwningCharacter->GetController(), this, BP_DamageType);
		}
	}

	DestructibleComponent->ApplyDamage(1, DestructibleComponent->GetComponentLocation(), FVector(1, 0, 0), 10000);
}

UDestructibleComponent* AThrowable_B::GetDestructibleComponent() const
{
	return DestructibleComponent;
}