// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_PlayerDropThrowable_B.h"
#include "Components/SkeletalMeshComponent.h"

#include "Player/PlayerCharacter_B.h"
#include "Components/HoldComponent_B.h"

void UAnimNotify_PlayerDropThrowable_B::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	UE_LOG(LogTemp, Warning, TEXT("Mesh Owner %s"), *GetNameSafe(MeshComp->GetOwner()));

	Player = Cast<APlayerCharacter_B>(MeshComp->GetOwner());
	if (Player)
		Player->HoldComponent->Drop();
}
