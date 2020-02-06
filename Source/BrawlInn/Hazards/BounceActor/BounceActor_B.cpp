// Fill out your copyright notice in the Description page of Project Settings.

#include "BounceActor_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "NiagaraComponent.h"


#include "BrawlInn.h"
#include "Hazards/BounceActor/BarrelTargetPoint_B.h"
#include "System/GameModes/GameMode_B.h"
#include "Characters/Player/PlayerCharacter_B.h"

ABounceActor_B::ABounceActor_B()
{
	PrimaryActorTick.bCanEverTick = false;

	PS_Explosion = CreateDefaultSubobject<UNiagaraComponent>("Stun Particle System");
	PS_Explosion->SetupAttachment(Mesh);
	PS_Explosion->SetRelativeLocation(FVector(0, 0,-55.f));
	PS_Explosion->SetWorldRotation(FRotator(90, 0, 0));
	PS_Explosion->SetAutoActivate(false);
}

void ABounceActor_B::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(ExplodeTime);
	OnDestroyed.AddDynamic(this, &ABounceActor_B::Explode);
}

void ABounceActor_B::Explode(AActor* DestroyedActor)
{
	if (PS_Explosion)
		PS_Explosion->Activate(true);
	else
		BWarn("No Explosion");

	//UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), DamageAmount, 0, GetActorLocation(), InnerRadius, Radius, Falloff, BP_DamageType, {}, this);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), DamageAmount,GetActorLocation(),Radius, BP_DamageType, {}, this,nullptr);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), InnerRadius, 10, FColor::Blue, false, 2.f);
	DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 10, FColor::Red, false, 2.f);
	if (Target)
		Target->SetActorHiddenInGame(true);

	//Respawns player. Kinda hates having this here.
	if (PlayerController)
	{
		AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->SpawnCharacter_D.Broadcast(PlayerController, true,FTransform(GetActorLocation()));
		}
	}


}
