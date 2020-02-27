// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_ChargePunch_B.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"

#include "BrawlInn.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Components/PunchComponent_B.h"

void UANS_ChargePunch_B::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	AnimLength = TotalDuration / (Animation->RateScale* (-1)); //Multiplies w -1 just cause the animation is reversed atm
	CurrentTime = 0.f;
	Player = Cast<APlayerCharacter_B>(MeshComp->GetOwner());
}

void UANS_ChargePunch_B::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (IsValid(Player) && IsValid(Player->PunchComponent))
	{
		CurrentTime += FrameDeltaTime;
		float ChargeTimer = (CurrentTime / AnimLength);

		if (!bChargeLevel1Reached)		//Tier 1
		{
			Player->PunchComponent->SetChargeLevel(EChargeLevel::EChargeLevel1);
			bChargeLevel1Reached = true;

		}
		else if ((ChargeTimer >= Player->PunchComponent->ChargeTier2Percentage) && (!bChargeLevel2Reached))	//Tier 2
		{
			Player->PunchComponent->SetChargeLevel(EChargeLevel::EChargeLevel2);
			bChargeLevel2Reached = true;
		}
		else if ((ChargeTimer >= Player->PunchComponent->ChargeTier3Percentage) && (!bChargeLevel3Reached))															//Tier 3
		{
			Player->PunchComponent->SetChargeLevel(EChargeLevel::EChargeLevel3);
			bChargeLevel3Reached = true;
		}
	}
}

void UANS_ChargePunch_B::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Player->PunchComponent->ChargeLevel = EChargeLevel::ENotCharging;

	bChargeLevel1Reached = false;
	bChargeLevel2Reached = false;
	bChargeLevel3Reached = false;
}