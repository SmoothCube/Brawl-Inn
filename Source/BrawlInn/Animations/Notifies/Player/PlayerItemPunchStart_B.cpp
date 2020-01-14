// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerItemPunchStart_B.h"
#include "Components/SkeletalMeshComponent.h"

#include "Player/PlayerCharacter_B.h"
#include "Components/PunchComponent_B.h"

void UPlayerItemPunchStart_B::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	Player = Cast<APlayerCharacter_B>(MeshComp->GetOwner());
	if (Player)
		Player->PunchComponent->ItemPunchEnd();
}
