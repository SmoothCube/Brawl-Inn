// Fill out your copyright notice in the Description page of Project Settings.


#include "CannonFired_B.h"
#include "Hazards/BounceActor/BounceActorSpawner_B.h"
#include "Components/SkeletalMeshComponent.h"
void UCannonFired_B::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	ABounceActorSpawner_B* Cannon = Cast<ABounceActorSpawner_B>(MeshComp->GetOwner());
	if (Cannon && Cannon->IsShooting())
		Cannon->SetIsShooting(false);
}