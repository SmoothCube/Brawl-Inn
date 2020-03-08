// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_ChargePunch_B.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"

#include "BrawlInn.h"
#include "Characters/Player/PlayerCharacter_B.h"

void UANS_ChargePunch_B::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	AnimLength =FMath::Abs( TotalDuration / (Animation->RateScale)); //does abs just cause the animation is reversed atm
	CurrentTime = 0.f;
	Player = Cast<APlayerCharacter_B>(MeshComp->GetOwner());
}

void UANS_ChargePunch_B::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (IsValid(Player))
	{
		CurrentTime += FrameDeltaTime;
		float ChargeTimer = (CurrentTime / AnimLength);

		if (!bChargeLevel1Reached)		//Tier 1
		{
			Player->SetChargeLevel(EChargeLevel::EChargeLevel1);
			bChargeLevel1Reached = true;

		}
		else if ((ChargeTimer >= Player->ChargeTier2Percentage) && (!bChargeLevel2Reached))	//Tier 2
		{
			Player->SetChargeLevel(EChargeLevel::EChargeLevel2);
			bChargeLevel2Reached = true;
		}
		else if ((ChargeTimer >= Player->ChargeTier3Percentage) && (!bChargeLevel3Reached))															//Tier 3
		{
			Player->SetChargeLevel(EChargeLevel::EChargeLevel3);
			bChargeLevel3Reached = true;
		}
	}
}

void UANS_ChargePunch_B::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (IsValid(Player))
	{
		Player->SetChargeLevel(EChargeLevel::ENotCharging);
	}

	bChargeLevel1Reached = false;
	bChargeLevel2Reached = false;
	bChargeLevel3Reached = false;
}