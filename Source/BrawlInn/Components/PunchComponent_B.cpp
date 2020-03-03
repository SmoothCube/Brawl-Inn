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
	BWarn("Punch Start!", *GetNameSafe(this));
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

	float f = OwningCharacter->GetCharacterMovement()->Velocity.Size() / OwningCharacter->GetCharacterMovement()->MaxWalkSpeed;
	float dist = (MaxPunchDashDistance - MinPunchDashDistance) * (int)ChargeLevel + MinPunchDashDistance;
	OwningCharacter->GetCharacterMovement()->AddForce(OwningCharacter->GetActorForwardVector() * dist * PunchDashForceModifier);
}
	
void UPunchComponent_B::Dash()
{
	if (bIsDashing)
		return;
	bIsDashing = true;

	OwningCharacter->GetCapsuleComponent()->SetCollisionProfileName("Capsule-Dash");

	OwningCharacter->MakeInvulnerable(0.3f, false);
	VelocityBeforeDash = OwningCharacter->GetCharacterMovement()->Velocity;

	FVector NormInput = OwningCharacter->InputVector.GetSafeNormal();
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
	BWarn("Punch End for %s", *GetNameSafe(OwningCharacter));

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
	BWarn("Punch Hit Player! %s", *GetNameSafe(OtherPlayer));

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
	BWarn("Punch Hit %s! ", *GetNameSafe(OtherComp));

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
		if (PlayerThatPunched->PunchComponent->ChargeLevel == EChargeLevel::EChargeLevel3)
			OwningCharacter->CheckFall(InPunchStrength);
		else if (PlayerThatPunched->PunchComponent->ChargeLevel == EChargeLevel::EChargeLevel2)
			OwningCharacter->AddStun(PlayerThatPunched->StunStrength * 2);
		else
			OwningCharacter->AddStun(PlayerThatPunched->StunStrength);

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
	FVector Strength;
	if (ChargeLevel == EChargeLevel::EChargeLevel3)
	{
		BWarn("Level 3 charge!");
		Strength = OwningCharacter->GetActorForwardVector() * Level3PunchStrength;
	}
	else if (ChargeLevel == EChargeLevel::EChargeLevel2)
	{
		BWarn("Level 2 charge!");
		Strength = OwningCharacter->GetActorForwardVector() * Level2PunchStrength;
	}
	else
	{
		BWarn("Level 1 charge!");
		Strength = OwningCharacter->GetActorForwardVector() * Level1PunchStrength;
	}
	
	return Strength;
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
	BWarn("Overlap Start! with %s, %s", *GetNameSafe(OtherComp), *GetNameSafe(OtherActor));


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