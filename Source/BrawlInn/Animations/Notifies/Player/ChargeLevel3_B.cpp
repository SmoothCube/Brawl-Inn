// Fill out your copyright notice in the Description page of Project Settings.


#include "ChargeLevel3_B.h"
#include "Components/SkeletalMeshComponent.h"

#include "BrawlInn.h"
#include "Characters/Player/PlayerCharacter_B.h"

void UChargeLevel3_B::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	APlayerCharacter_B* Player = Cast<APlayerCharacter_B>(MeshComp->GetOwner());
	if (Player && Player->IsCharging())
		Player->SetChargeLevel(EChargeLevel::EChargeLevel3);
}