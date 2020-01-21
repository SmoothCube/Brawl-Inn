// Fill out your copyright notice in the Description page of Project Settings.


#include "PunchComponent_B.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "BrawlInn.h"
#include "TimerManager.h"

#include "Player/PlayerCharacter_B.h"
#include "Player/PlayerController_B.h"

// Sets default values for this component's properties
UPunchComponent_B::UPunchComponent_B()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Player = Cast<APlayerCharacter_B>(GetOwner());
}


// Called when the game starts
void UPunchComponent_B::BeginPlay()
{
	Super::BeginPlay();
	OnComponentBeginOverlap.AddDynamic(this, &UPunchComponent_B::OnOverlapBegin);
}

// Called every frame
void UPunchComponent_B::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPunchComponent_B::PunchStart()
{
	if (!Player) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::Punch]: %s No Player found for PunchComponent!"), *GetNameSafe(this)); return; }

	Player->MakeVulnerable();
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//Thought this would fix bug #39
	TArray<UPrimitiveComponent*> OverlappingComponents;
	GetOverlappingComponents(OverlappingComponents);
	for (auto& comp : OverlappingComponents)
	{
		AActor* OtherActor = comp->GetOwner();
		APlayerCharacter_B* OtherPlayer = Cast<APlayerCharacter_B>(OtherActor);
		UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(comp);

		if (!bHasHit && OtherActor != GetOwner())
		{
			if (OtherPlayer != nullptr && Capsule != nullptr)
				PunchHit(OtherPlayer);
			else
				PunchHit(comp);
		}
	}

	Dash();
}

void UPunchComponent_B::Dash()
{
	//Dash Logic
	bIsDashing = true;

	//This way, f is 0 if the player is looking behind him or standing still, 0.5 if they are facing sideways, and 1 if facing forward.
	float f = -1;
	if (!(Player->GetCharacterMovement()->Velocity.IsNearlyZero()))
	{
		f = FVector::DotProduct(Player->GetActorForwardVector(), Player->GetCharacterMovement()->Velocity.GetSafeNormal());
	}
	f += 1;
	f /= 2;

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
	Player->GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	Player->GetCharacterMovement()->Velocity = Player->GetCharacterMovement()->Velocity.GetClampedToMaxSize(1000.f);

	GetWorld()->GetTimerManager().SetTimer(
		TH_PunchAgainHandle,
		this,
		&UPunchComponent_B::setIsPunchingFalse,
		PunchWaitingTime,
		false);
}

void UPunchComponent_B::setIsPunchingFalse()
{
	bIsPunching = false;
	bHasHit = false;
}

void UPunchComponent_B::PunchHit(APlayerCharacter_B* OtherPlayer)
{
	if (!OtherPlayer) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::PunchHit]: %s No OtherPlayer found!"), *GetNameSafe(this)); return; }
	if (!OtherPlayer->PunchComponent) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::PunchHit]: No PunchComponent found for OtherPlayer %s!"), *GetNameSafe(OtherPlayer)); return; }
	if (!Player) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::PunchHit]: No Player found for PunchComponent %s!"), *GetNameSafe(this)); return; }

	//has the invulnerability check here to be able to do other stuff when you hit someone invulnerable
	if (!OtherPlayer->bIsInvulnerable)
	{
		OtherPlayer->PunchComponent->GetPunched(CalculatePunchStrenght(),OtherPlayer);
		Player->CurrentFallTime = 0.f;
		Player->GetMovementComponent()->Velocity *= PunchHitVelocityDamper;
		if (Player->PlayerController)
			Player->PlayerController->PlayControllerVibration(0.7, 0.3, true, true, true, true);
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

	Player->CurrentFallTime = 0.f;
	Player->GetMovementComponent()->Velocity *= PunchHitVelocityDamper;
	OtherComp->AddImpulse(CalculatePunchStrenght());
	bHasHit = true;

}

bool UPunchComponent_B::GetIsPunching()
{
	return bIsPunching;
}

void UPunchComponent_B::GetPunched(FVector InPunchStrength, APlayerCharacter_B* PlayerThatPunched)
{
	if (!Player) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::GetPunched]: No Player found for PunchComponent %s!"), *GetNameSafe(this)); return; }

	float strength = InPunchStrength.Size();
	GetPunched_D.Broadcast(PlayerThatPunched);
	BWarn("Getting Punched with strength: %s", *InPunchStrength.ToString());

	PunchEnd();
	//Player->Fall();
	Player->GetCharacterMovement()->AddImpulse(InPunchStrength);
	Player->GetCharacterMovement()->AddImpulse(InPunchStrength);
}

//calculates the punch strength for the player. Has to be used by the puncher.
FVector UPunchComponent_B::CalculatePunchStrenght()
{
	if (!Player) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::GetPunched]: No Player found for PunchComponent %s!"), *GetNameSafe(this)); return FVector(); }
	FVector Strength;
	if (!Player->GetVelocity().IsNearlyZero())
	{
		Strength= BasePunchStrength * Player->GetVelocity().GetSafeNormal() + Player->GetVelocity() * PunchStrengthMultiplier;
	}
	else
	{
		Strength = Player->GetActorForwardVector()*BasePunchStrength;
	}
	BWarn("Strength: %s", *Strength.ToString())
	return Strength;
}

void UPunchComponent_B::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	APlayerCharacter_B* OtherPlayer = Cast<APlayerCharacter_B>(OtherActor);
	UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(OtherComp);

	if (!bHasHit && OtherActor != GetOwner())
	{
		if (OtherPlayer != nullptr && Capsule != nullptr)
			PunchHit(OtherPlayer);
		else
			PunchHit(OtherComp);
	}
}
