// Fill out your copyright notice in the Description page of Project Settings.

#include "PunchComponent_B.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"

#include "BrawlInn.h"
#include "Characters/Character_B.h"
#include "System/GameInstance_B.h"

UPunchComponent_B::UPunchComponent_B()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPunchComponent_B::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacter_B>(GetOwner());

	OnComponentBeginOverlap.AddDynamic(this, &UPunchComponent_B::OnOverlapBegin);
}

void UPunchComponent_B::PunchStart()
{
	if (!OwningCharacter) { BError("%s No OwningCharacter found for PunchComponent!", *GetNameSafe(this)); return; }

	OwningCharacter->MakeVulnerable();

	SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	TArray<UPrimitiveComponent*> OurOverlappingComponents;
	GetOverlappingComponents(OurOverlappingComponents);
	for (auto& comp : OurOverlappingComponents)
	{
		ACharacter_B* OtherCharacter = Cast<ACharacter_B>(comp->GetOwner());
		UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(comp);

		if (!bHasHit && OtherCharacter != OwningCharacter)
		{
			if (OtherCharacter != nullptr && Capsule != nullptr)
				PunchHit(OtherCharacter);
			else
				PunchHit(comp);
		}
	}

	if (PunchSound)
	{
		float volume = 1.f;
		UGameInstance_B* GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (GameInstance)
		{
			volume *= GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume();
		}
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			PunchSound,
			GetComponentLocation(),
			volume
		);
	}

	PunchDash();
}

void UPunchComponent_B::PunchDash()
{
	VelocityBeforeDash = OwningCharacter->GetCharacterMovement()->Velocity;

	float f = OwningCharacter->GetCharacterMovement()->Velocity.Size() / OwningCharacter->GetCharacterMovement()->MaxWalkSpeed;
	float dist = (MaxPunchDashDistance - MinPunchDashDistance) * f + MinPunchDashDistance;
	OwningCharacter->GetCharacterMovement()->AddForce(OwningCharacter->GetActorForwardVector() * dist * DashForceModifier);
}

void UPunchComponent_B::Dash()
{
	OwningCharacter->MakeInvulnerable(0.3f, false);
	VelocityBeforeDash = OwningCharacter->GetCharacterMovement()->Velocity;

	FVector NormInput = OwningCharacter->InputVector.GetSafeNormal();
	if (NormInput.IsNearlyZero())
	{
		OwningCharacter->GetCharacterMovement()->AddForce(OwningCharacter->GetActorForwardVector() * DashDistance);
	}
	else
	{
		OwningCharacter->GetCharacterMovement()->AddForce(OwningCharacter->InputVector * DashDistance);
		OwningCharacter->SetActorRotation(OwningCharacter->InputVector.Rotation());

	}

}

void UPunchComponent_B::PunchEnd()
{
	if (!bIsPunching) { return; }
	if (!OwningCharacter) { BError("%s No OwningCharacter found for PunchComponent!", *GetNameSafe(this)); return; }

	SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OwningCharacter->GetCharacterMovement()->MaxWalkSpeed = OwningCharacter->NormalMaxWalkSpeed;
	OwningCharacter->GetCharacterMovement()->Velocity = OwningCharacter->GetCharacterMovement()->Velocity.GetClampedToMaxSize(OwningCharacter->NormalMaxWalkSpeed * PostDashRemainingVelocityPercentage);

	GetWorld()->GetTimerManager().SetTimer(
		TH_PunchAgainHandle,
		[&]()
		{
			bIsPunching = false;
			bHasHit = false;
		},
		PunchWaitingTime,
			false);
	VelocityBeforeDash = FVector::ZeroVector;
}

void UPunchComponent_B::PunchHit(ACharacter_B* OtherPlayer)
{
	if (!OtherPlayer) { BError("%s No OtherPlayer found!", *GetNameSafe(this)); return; }
	if (!OtherPlayer->PunchComponent) { BError("No PunchComponent found for OtherPlayer %s!", *GetNameSafe(OtherPlayer)); return; }
	if (!OwningCharacter) { BError("No OwningCharacter found for PunchComponent %s!", *GetNameSafe(this)); return; }

	if (!OtherPlayer->bIsInvulnerable || !OtherPlayer->bHasShield)
	{
		OtherPlayer->PunchComponent->GetPunched(CalculatePunchStrength(), OwningCharacter);
		UGameplayStatics::ApplyDamage(OtherPlayer, CalculatePunchDamage(OtherPlayer), OwningCharacter->GetController(), OwningCharacter, BP_DamageType);

		OwningCharacter->StunStrength = 1; // This ends the punch powerup after you hit a punch. If we want to end the effect after every punch we need to move this to PunchEnd
		OwningCharacter->GetMovementComponent()->Velocity *= PunchHitVelocityDamper;

		OnPunchHit_D.Broadcast();

		bHasHit = true;
	}
	else
	{
		OwningCharacter->GetCharacterMovement()->Velocity *= -0.5;
	}
}

void UPunchComponent_B::PunchHit(UPrimitiveComponent* OtherComp)
{
	if (!OtherComp) { BError("%s No OtherPlayer found!", *GetNameSafe(this)); return; }
	if (!OwningCharacter) { BError("No OwningCharacter found for PunchComponent %s!", *GetNameSafe(this)); return; }
	if (OtherComp->IsSimulatingPhysics())
	{
		OwningCharacter->GetMovementComponent()->Velocity *= PunchHitVelocityDamper;
		OtherComp->AddImpulse(CalculatePunchStrength());
	}
	OnPunchHit_D.Broadcast();
	bHasHit = true;
}

float UPunchComponent_B::CalculatePunchDamage(ACharacter_B* OtherPlayer)
{
	float f = VelocityBeforeDash.Size() / (OwningCharacter->GetCharacterMovement()->MaxWalkSpeed * OtherPlayer->FallLimitMultiplier);
	f = FMath::Clamp(f, 0.f, 1.f);
	float TotalDamage = 5 + (30 * (f));
	return TotalDamage;
}

bool UPunchComponent_B::GetIsPunching()
{
	return bIsPunching;
}

void UPunchComponent_B::GetPunched(FVector InPunchStrength, ACharacter_B* PlayerThatPunched)
{
	if (!OwningCharacter) { BError("No OwningCharacter found for PunchComponent %s!", *GetNameSafe(this)); return; }

	float Strength = InPunchStrength.Size();
	OnGetPunched_D.Broadcast(PlayerThatPunched);

	PunchEnd();

	if (!OwningCharacter->IsInvulnerable())
	{
		OwningCharacter->GetCharacterMovement()->AddImpulse(InPunchStrength);
		OwningCharacter->AddStun(PlayerThatPunched->StunStrength);
		OwningCharacter->RemoveShield();
	}
}


FVector UPunchComponent_B::CalculatePunchStrength()
{
	if (!OwningCharacter) { BError("No OwningCharacter found for PunchComponent %s!", *GetNameSafe(this)); return FVector(); }
	FVector Strength;
	if (!VelocityBeforeDash.IsNearlyZero())
		Strength = BasePunchStrength * VelocityBeforeDash.GetSafeNormal() + VelocityBeforeDash * PunchStrengthMultiplier;
	else
		Strength = OwningCharacter->GetActorForwardVector() * BasePunchStrength;
	return Strength;
}

void UPunchComponent_B::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter_B* OtherPlayer = Cast<ACharacter_B>(OtherActor);
	UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(OtherComp);

	if (!bHasHit && OtherActor != OwningCharacter)
	{
		if (OtherPlayer != nullptr && Capsule != nullptr)
			PunchHit(OtherPlayer);
		else
			PunchHit(OtherComp);
	}
}
