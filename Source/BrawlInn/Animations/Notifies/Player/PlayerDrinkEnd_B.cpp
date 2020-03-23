// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDrinkEnd_B.h"
#include "Components/SkeletalMeshComponent.h"

#include "BrawlInn.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Components/ThrowComponent_B.h"


void UPlayerDrinkEnd_B::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//Super::Notify(MeshComp, Animation);
	//Player = Cast<APlayerCharacter_B>(MeshComp->GetOwner());
	//if (Player)
	//	Player->ThrowComponent->CancelDrinking();
}
