// Fill out your copyright notice in the Description page of Project Settings.


#include "CannonFired_B.h"
#include "Hazards/BounceActor/BounceActorSpawner_B.h"
#include "Components/SkeletalMeshComponent.h"

#include "BrawlInn.h"
void UCannonFired_B::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	BWarn("CannonFired Notify!");
	Super::Notify(MeshComp, Animation);
	//ABounceActorSpawner_B* Cannon = Cast<ABounceActorSpawner_B>(MeshComp->GetOwner());
	//
	//if (Cannon)
	//	Cannon->SetAnimationFinishedTimer();
}