// Fill out your copyright notice in the Description page of Project Settings.

#include "BTS_FleeFromPlayer_B.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "TimerManager.h"

#include "Characters/AI/AICharacter_B.h"
#include "Characters/AI/AIController_B.h"
#include "Characters/Player/PlayerCharacter_B.h"

UBTS_FleeFromPlayer_B::UBTS_FleeFromPlayer_B()
{
	bCreateNodeInstance = true;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

void UBTS_FleeFromPlayer_B::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	AAIController_B* Controller = Cast<AAIController_B>(OwnerComp.GetAIOwner());
	if (!Controller)
		return;

	Character = Cast<AAICharacter_B>(Controller->GetCharacter());

	if (!Character)
		return;

	Character->GetPawnSensingComponent()->OnHearNoise.AddDynamic(this, &UBTS_FleeFromPlayer_B::OnHearNoise);

	NormalMaxAcceleration = Character->GetCharacterMovement()->GetMaxAcceleration();
	NormalMaxWalkSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
}

void UBTS_FleeFromPlayer_B::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
	GetWorld()->GetTimerManager().ClearTimer(TH_ResetSpeed);
}

void UBTS_FleeFromPlayer_B::OnHearNoise(APawn* Instigator, const FVector& Location, float Volume)
{
	APlayerCharacter_B* PlayerCharacter = Cast<APlayerCharacter_B>(Instigator);
	if (!PlayerCharacter || !Character)
		return;

	Character->GetCharacterMovement()->MaxAcceleration = FleeMaxAcceleration;
	Character->GetCharacterMovement()->MaxWalkSpeed = FleeMaxWalkSpeed;

	GetWorld()->GetTimerManager().SetTimer(TH_ResetSpeed, [&]()
		{
			Character->GetCharacterMovement()->MaxAcceleration = NormalMaxAcceleration;
			Character->GetCharacterMovement()->MaxWalkSpeed = NormalMaxWalkSpeed;
		}, ResetSpeedTime, false);

}
