// Fill out your copyright notice in the Description page of Project Settings.


#include "AOEDamageArea_B.h"
#include "Player/PlayerCharacter_B.h"
#include "BrawlInn.h"
#include "Kismet/GameplayStatics.h"

AAOEDamageArea_B::AAOEDamageArea_B()
{
	SetActorTickInterval(1);
}

void AAOEDamageArea_B::BeginPlay()
{
	Super::BeginPlay();

}

void AAOEDamageArea_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> Players;
	GetOverlappingActors(Players, APlayerCharacter_B::StaticClass());

	for (const auto& Player : Players)
	{
		UGameplayStatics::ApplyDamage(Player, 1, Cast<APlayerCharacter_B>(Player)->GetController(), this, AOEDamageType);
	}
}
