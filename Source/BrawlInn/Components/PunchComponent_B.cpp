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
	bIsCharging = false;
	bIsPunching = true;
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

	float PunchDashSpeed = 0.f;
	switch (ChargeLevel)
	{
	case EChargeLevel::EChargeLevel1: PunchDashSpeed = Charge1PunchDashSpeed;
		break;
	case EChargeLevel::EChargeLevel2: PunchDashSpeed = Charge2PunchDashSpeed;
		break;
	case EChargeLevel::EChargeLevel3: PunchDashSpeed = Charge3PunchDashSpeed;
		break;
	default:
		break;
	}

	OwningCharacter->GetCharacterMovement()->Velocity = FVector(OwningCharacter->GetActorForwardVector() * PunchDashSpeed);

}
	
void UPunchComponent_B::Dash()
{
	if (bIsDashing)
		return;
	bIsDashing = true;

	OwningCharacter->GetCapsuleComponent()->SetCollisionProfileName("Capsule-Dash");

	OwningCharacter->MakeInvulnerable(0.3f, false);
	VelocityBeforeDash = OwningCharacter->GetCharacterMovement()->Velocity;

	const FVector NormInput = OwningCharacter->InputVector.GetSafeNormal();
	if (NormInput.IsNearlyZero())
	{
		//think setting velocity is more predictable and therefore better than adding force here.
		OwningCharacter->GetCharacterMovement()->Velocity = FVector(OwningCharacter->GetActorForwardVector() * DashSpeed);
	}
	else
	{
		OwningCharacter->GetCharacterMovement()->Velocity = FVector(OwningCharacter->InputVector * DashSpeed);
		OwningCharacter->SetActorRotation(OwningCharacter->InputVector.Rotation());
	}

	GetWorld()->GetTimerManager().SetTimer(
		TH_DashAgainHandle,
		[&]()
		{
			bIsDashing = false;
		},
		DashCooldown,
		false);

	GetWorld()->GetTimerManager().SetTimer(
		TH_DashDoneHandle,
		[&]() 
		{
			OwningCharacter->GetCapsuleComponent()->SetCollisionProfileName("Capsule");
			OwningCharacter->GetCharacterMovement()->Velocity = OwningCharacter->GetCharacterMovement()->Velocity * PostDashRemainingVelocityPercentage;
		},	
		DashTime,
	false);

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

void UPunchComponent_B::GetPunched(FVector InPunchStrength, ACharacter_B* PlayerThatPunched)
{
	if (!OwningCharacter) { BError("No OwningCharacter found for PunchComponent %s!", *GetNameSafe(this)); return; }

	float Strength = InPunchStrength.Size();
	OnGetPunched_D.Broadcast(PlayerThatPunched);

	PunchEnd(); //In case the player got hit while punching, mostly to make sure punch collisions get turned off
	if (!OwningCharacter->IsInvulnerable())
	{
		OwningCharacter->GetCharacterMovement()->AddImpulse(InPunchStrength);

		OwningCharacter->RemoveShield();

		switch (PlayerThatPunched->PunchComponent->ChargeLevel)
		{
		case EChargeLevel::EChargeLevel1:
			OwningCharacter->AddStun(PlayerThatPunched->StunStrength);
			break;
		case EChargeLevel::EChargeLevel2:
			OwningCharacter->AddStun(PlayerThatPunched->StunStrength * 2);
			break;
		case EChargeLevel::EChargeLevel3:
			OwningCharacter->CheckFall(InPunchStrength);
			return;
		default:
			break;
		}

		if (OwningCharacter->StunAmount >= OwningCharacter->PunchesToStun)
		{
			OwningCharacter->CheckFall(InPunchStrength);
		}
	}
}

void UPunchComponent_B::SetChargeLevel(EChargeLevel chargeLevel)
{

	ChargeLevel = chargeLevel;
	bool ShouldPlaySound = true;
	float SoundPitch = 1.0f; 
	if (!IsValid(OwningCharacter))
		return;
	switch (ChargeLevel)
	{
	case EChargeLevel::ENotCharging:
		OwningCharacter->RotationInterpSpeed = OwningCharacter->NormalRotationInterpSpeed;
		OwningCharacter->GetCharacterMovement()->MaxWalkSpeed = OwningCharacter->NormalMaxWalkSpeed;
		ShouldPlaySound = false;
		break;
	case EChargeLevel::EChargeLevel1:
		OwningCharacter->RotationInterpSpeed = OwningCharacter->Charge1RotSpeed;
		OwningCharacter->GetCharacterMovement()->MaxWalkSpeed = Charge1MoveSpeed;
		OwningCharacter->GetCharacterMovement()->Velocity = OwningCharacter->GetVelocity().GetClampedToMaxSize(Charge1MoveSpeed);
		ShouldPlaySound = false;
		SoundPitch = 0.8f;
		break;
	case EChargeLevel::EChargeLevel2:
		OwningCharacter->RotationInterpSpeed = OwningCharacter->Charge2RotSpeed;
		OwningCharacter->GetCharacterMovement()->MaxWalkSpeed = Charge2MoveSpeed;
		OwningCharacter->GetCharacterMovement()->Velocity = OwningCharacter->GetVelocity().GetClampedToMaxSize(Charge2MoveSpeed);
		SoundPitch = 1.0f;
		break;
	case EChargeLevel::EChargeLevel3:
		OwningCharacter->RotationInterpSpeed = OwningCharacter->Charge3RotSpeed;
		OwningCharacter->GetCharacterMovement()->MaxWalkSpeed = Charge3MoveSpeed;
		OwningCharacter->GetCharacterMovement()->Velocity = OwningCharacter->GetVelocity().GetClampedToMaxSize(Charge3MoveSpeed);
		SoundPitch = 1.2f;
		break;
	default:
		OwningCharacter->RotationInterpSpeed = OwningCharacter->NormalRotationInterpSpeed;
		OwningCharacter->GetCharacterMovement()->MaxWalkSpeed = OwningCharacter->NormalMaxWalkSpeed;
		ShouldPlaySound = false;
		break;
	}

	if (ShouldPlaySound && ChargePunchSound)
	{
		float volume = 1.f;
		UGameInstance_B* GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (GameInstance)
		{
			volume *= GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume();
		}
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ChargePunchSound,
			GetComponentLocation(),
			volume,
			SoundPitch
		);
	}
}

FVector UPunchComponent_B::CalculatePunchStrength()
{
	if (!OwningCharacter) { BError("No OwningCharacter found for PunchComponent %s!", *GetNameSafe(this)); return FVector(); }

	switch (ChargeLevel)
	{
	case EChargeLevel::EChargeLevel1:
		return OwningCharacter->GetActorForwardVector() * Level1PunchStrength;
	case EChargeLevel::EChargeLevel2: 
		return OwningCharacter->GetActorForwardVector() * Level2PunchStrength;
	case EChargeLevel::EChargeLevel3: 
		return OwningCharacter->GetActorForwardVector() * Level3PunchStrength;
	default:
		return FVector::ZeroVector;
	}
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

void UPunchComponent_B::SetIsPunching(bool Value)
{
	bIsPunching = Value;
}

bool UPunchComponent_B::GetIsCharging()
{
	return bIsCharging;
}

void UPunchComponent_B::SetIsCharging(bool Value)
{
	bIsCharging = Value;
}

bool UPunchComponent_B::GetIsDashing()
{
	return bIsDashing;
}

void UPunchComponent_B::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter_B* OtherPlayer = Cast<ACharacter_B>(OtherActor);
	UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(OtherComp);

	if (!bHasHit && OtherActor != OwningCharacter)
	{
		if (IsValid(OtherPlayer))
		{
			if(IsValid(Capsule))
				PunchHit(OtherPlayer);
		}
		else
		{
			PunchHit(OtherComp);
		}
	}
}