// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_ChargeThrow_B.h"
#include "Components/SkeletalMeshComponent.h"

#include "System/Enums/Charge.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Components/ThrowComponent_B.h"
#include "Animation/AnimSequence.h"
#include "BrawlInn.h"

void UANS_ChargeThrow_B::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	AnimLength = TotalDuration/ Animation->RateScale;
	CurrentTime = 0.f;
	Player = Cast<APlayerCharacter_B>(MeshComp->GetOwner());
}

void UANS_ChargeThrow_B::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (IsValid(Player) && Player->IsCharging())
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

void UANS_ChargeThrow_B::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	bChargeLevel1Reached = false;
	bChargeLevel2Reached = false;
	bChargeLevel3Reached = false;
}
