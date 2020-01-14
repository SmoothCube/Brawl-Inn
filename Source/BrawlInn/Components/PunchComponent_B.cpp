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

	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
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
		f = FVector::DotProduct(Player->RotationVector.GetSafeNormal(), Player->GetCharacterMovement()->Velocity.GetSafeNormal());
	}
	f += 1;
	f /= 2;

	float dist = (MaxDashDistance-MinDashDistance)*f + MinDashDistance;
	Player->GetCharacterMovement()->AddForce(Player->GetActorForwardVector() * dist* DashForceModifier);
	
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

	BWarn("[UPunchComponent_B::PunchHit] Sphere Overlapped! Other Actor: %s", *GetNameSafe(this));
	OtherPlayer->PunchComponent->GetPunched(Player->GetVelocity());
	Player->CurrentFallTime = 0.f;
	Player->GetMovementComponent()->Velocity *= PunchHitVelocityDamper;
	Player->PlayerController->PlayControllerVibration(0.7, 0.3, true, true, true, true);
	bHasHit = true;
	
}

void UPunchComponent_B::PunchHit(UPrimitiveComponent* OtherComp)
{
	if (!OtherComp) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::PunchHit]: %s No OtherPlayer found!"), *GetNameSafe(this)); return; }
	if (!Player) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::PunchHit]: No Player found for PunchComponent %s!"), *GetNameSafe(this)); return; }

	BWarn("[UPunchComponent_B::PunchHit] Sphere Overlapped! Other Actor: %s", *GetNameSafe(this));
	Player->CurrentFallTime = 0.f;
	Player->GetMovementComponent()->Velocity *= PunchHitVelocityDamper;
	OtherComp->AddImpulse(Player->GetVelocity() * BasePunchStrength);
	bHasHit = true;

}

bool UPunchComponent_B::GetIsPunching()
{
	return bIsPunching;
}

void UPunchComponent_B::GetPunched(FVector InPunchStrength)
{
	if (!Player) { UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent::GetPunched]: No Player found for PunchComponent %s!"), *GetNameSafe(this)); return; }
	float strength = InPunchStrength.Size();
	if (strength > MinPunchStrengthToFall)
	{
		PunchEnd();
		//Player->Fall();
		Player->GetCharacterMovement()->AddImpulse(InPunchStrength * BasePunchStrength);
	}
	else
	{
		Player->GetCharacterMovement()->AddImpulse(InPunchStrength * BasePunchStrength);
	}
}

void UPunchComponent_B::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	APlayerCharacter_B* OtherPlayer = Cast<APlayerCharacter_B>(OtherActor);
	UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(OtherComp);

	if (!bHasHit && OtherActor != GetOwner())
	{
		if(OtherPlayer != nullptr && Capsule != nullptr)
			PunchHit(OtherPlayer);
		else
			PunchHit(OtherComp);
	}
}
