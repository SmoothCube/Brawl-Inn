// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerThrow_B.h"

#include "BrawlInn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Components/ThrowComponent_B.h"

void UPlayerThrow_B::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	Player = Cast<APlayerCharacter_B>(MeshComp->GetOwner());
	if (Player)
		Player->ThrowComponent->Throw();
	BWarn("Notify Throw!");
}
