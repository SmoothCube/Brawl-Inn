// Fill out your copyright notice in the Description page of Project Settings.

#include "PunchComponent_B.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BrawlInn.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"

#include "Characters/Character_B.h"
#include "Characters/Player/PlayerController_B.h"
#include "System/DamageTypes/Punch_DamageType_B.h"
#include "System/GameInstance_B.h"

// Sets default values for this component's properties
UPunchComponent_B::UPunchComponent_B()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	Player = Cast<ACharacter_B>(GetOwner());
}

// Called when the game starts
void UPunchComponent_B::BeginPlay()
{
	Super::BeginPlay();
	OnComponentBeginOverlap.AddDynamic(this, &UPunchComponent_B::OnOverlapBegin);
}

void UPunchComponent_B::PunchStart()
{
	if (!Player) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::Punch]: %s No Player found for PunchComponent!"), *GetNameSafe(this)); return; }

	Player->MakeVulnerable();
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//Thought this would fix bug #39
	TArray<UPrimitiveComponent*> OurOverlappingComponents;
	GetOverlappingComponents(OurOverlappingComponents);
	for (auto& comp : OurOverlappingComponents)
	{
		AActor* OtherActor = comp->GetOwner();
		ACharacter_B* OtherPlayer = Cast<ACharacter_B>(OtherActor);
		UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(comp);

		if (!bHasHit && OtherActor != GetOwner())
		{
			if (OtherPlayer != nullptr && Capsule != nullptr)
				PunchHit(OtherPlayer);
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
			volume *= GameInstance->MasterVolume * GameInstance->SfxVolume;
		}
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			PunchSound,
			GetComponentLocation(),
			volume
		);
	}

	Dash();
}

void UPunchComponent_B::Dash()
{
	//Dash Logic
	bIsDashing = true;
	VelocityBeforeDash = Player->GetCharacterMovement()->Velocity;

	float f = Player->GetCharacterMovement()->Velocity.Size() / Player->GetCharacterMovement()->MaxWalkSpeed;
	float dist = (MaxDashDistance - MinDashDistance) * f + MinDashDistance;
	Player->GetCharacterMovement()->AddForce(Player->GetActorForwardVector() * dist * DashForceModifier);

	//UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent_B::Dash] Dashing: f: %f, dist: %f"), f, dist);
}

void UPunchComponent_B::PunchEnd()
{
	//just to see if we want to run the function	
	if (!bIsPunching) { return; }
	if (!Player) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::PunchEnd]: No Player found for PunchComponent %s!"), *GetNameSafe(this)); return; }

	SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Dash ends
	Player->GetCharacterMovement()->MaxWalkSpeed = Player->NormalMaxWalkSpeed;
	Player->GetCharacterMovement()->Velocity = Player->GetCharacterMovement()->Velocity.GetClampedToMaxSize(Player->NormalMaxWalkSpeed*PostDashRemainingVelocityPercentage);
	
	GetWorld()->GetTimerManager().SetTimer(
		TH_PunchAgainHandle,
		this,
		&UPunchComponent_B::setIsPunchingFalse,
		PunchWaitingTime,
		false);
	VelocityBeforeDash = FVector::ZeroVector;
}

void UPunchComponent_B::setIsPunchingFalse()
{
	bIsPunching = false;
	bHasHit = false;
}

void UPunchComponent_B::PunchHit(ACharacter_B* OtherPlayer)
{
	if (!OtherPlayer) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::PunchHit]: %s No OtherPlayer found!"), *GetNameSafe(this)); return; }
	if (!OtherPlayer->PunchComponent) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::PunchHit]: No PunchComponent found for OtherPlayer %s!"), *GetNameSafe(OtherPlayer)); return; }
	if (!Player) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::PunchHit]: No Player found for PunchComponent %s!"), *GetNameSafe(this)); return; }

	//has the invulnerability check here to be able to do other stuff when you hit someone invulnerable
	if (!OtherPlayer->bIsInvulnerable || !OtherPlayer->bHasShield)
	{

		OtherPlayer->PunchComponent->GetPunched(CalculatePunchStrenght(),Player);
		UGameplayStatics::ApplyDamage(OtherPlayer, CalculatePunchDamage(OtherPlayer), Player->GetController(), Player, BP_DamageType);
		
		Player->StunStrength = 1; // This ends the punch powerup after you hit a punch. If we want to end the effect after every punch we need to move this to PunchEnd
		Player->GetMovementComponent()->Velocity *= PunchHitVelocityDamper;
		if (Player->PlayerController)Player->PlayerController->PlayControllerVibration(0.2f, 0.3f, true, true, true, true);
			
		bHasHit = true;
	}
	else
	{
		Player->GetCharacterMovement()->Velocity *= -0.5;
	}
}

void UPunchComponent_B::PunchHit(UPrimitiveComponent* OtherComp)
{
	if (!OtherComp) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::PunchHit]: %s No OtherPlayer found!"), *GetNameSafe(this)); return; }
	if (!Player) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::PunchHit]: No Player found for PunchComponent %s!"), *GetNameSafe(this)); return; }
	if (OtherComp->IsSimulatingPhysics())
	{
		Player->GetMovementComponent()->Velocity *= PunchHitVelocityDamper;
		OtherComp->AddImpulse(CalculatePunchStrenght());
	}
	bHasHit = true;
}

float UPunchComponent_B::CalculatePunchDamage(ACharacter_B* OtherPlayer)
{
	float f = VelocityBeforeDash.Size() / (Player->GetCharacterMovement()->MaxWalkSpeed * OtherPlayer->FallLimitMultiplier);
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
	if (!Player) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::GetPunched]: No Player found for PunchComponent %s!"), *GetNameSafe(this)); return; }

	float strength = InPunchStrength.Size();
	GetPunched_D.Broadcast(PlayerThatPunched);

	PunchEnd();
	//Player->Fall();
	if (!Player->IsInvulnerable())
	{
		Player->GetCharacterMovement()->AddImpulse(InPunchStrength);
		Player->AddStun(PlayerThatPunched->StunStrength);
	}
	Player->RemoveShield();
}

//calculates the punch strength for the player. Has to be used by the puncher.
FVector UPunchComponent_B::CalculatePunchStrenght()
{
	if (!Player) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::GetPunched]: No Player found for PunchComponent %s!"), *GetNameSafe(this)); return FVector(); }
	FVector Strength;
	if (!VelocityBeforeDash.IsNearlyZero())
	{
		Strength= BasePunchStrength * VelocityBeforeDash.GetSafeNormal() + VelocityBeforeDash * PunchStrengthMultiplier;
	}
	else
	{
		Strength = Player->GetActorForwardVector()*BasePunchStrength;
	}
	return Strength;
}

void UPunchComponent_B::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter_B* OtherPlayer = Cast<ACharacter_B>(OtherActor);
	UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(OtherComp);

	if (!bHasHit && OtherActor != GetOwner())
	{
		if (OtherPlayer != nullptr && Capsule != nullptr)
			PunchHit(OtherPlayer);
		else
			PunchHit(OtherComp);
	}
}
