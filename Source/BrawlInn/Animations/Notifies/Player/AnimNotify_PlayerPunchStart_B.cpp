// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_PlayerPunchStart_B.h"
#include "Components/SkeletalMeshComponent.h"

#include "Player/PlayerCharacter_B.h"
#include "Components/PunchComponent_B.h"

void UAnimNotify_PlayerPunchStart_B::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	UE_LOG(LogTemp, Warning, TEXT("Mesh Owner %s"), *GetNameSafe(MeshComp->GetOwner()));
	
	Player = Cast<APlayerCharacter_B>(MeshComp->GetOwner());
	if (Player)
		Player->PunchComponent->PunchStart();
}
