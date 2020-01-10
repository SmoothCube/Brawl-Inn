// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_PlayerThrow_B.h"
#include "Components/SkeletalMeshComponent.h"

#include "Player/PlayerCharacter_B.h"
#include "Components/ThrowComponent_B.h"

void UAnimNotify_PlayerThrow_B::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	UE_LOG(LogTemp, Warning, TEXT("Mesh Owner %s"), *GetNameSafe(MeshComp->GetOwner()));

	Player = Cast<APlayerCharacter_B>(MeshComp->GetOwner());
	if (Player)
		Player->ThrowComponent->Throw();

}
