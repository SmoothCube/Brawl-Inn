// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPunchStart_B.h"
#include "Components/SkeletalMeshComponent.h"

#include "BrawlInn.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Components/PunchComponent_B.h"

void UPlayerPunchStart_B::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	BWarn("PunchStart Notify!");

	Player = Cast<APlayerCharacter_B>(MeshComp->GetOwner());
	if (Player)
		Player->PunchComponent->PunchStart();
}
