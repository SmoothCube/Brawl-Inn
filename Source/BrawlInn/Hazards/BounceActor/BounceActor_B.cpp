// Fill out your copyright notice in the Description page of Project Settings.


#include "BounceActor_B.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "BrawlInn.h"

#include "Player/PlayerCharacter_B.h"
// Sets default values
ABounceActor_B::ABounceActor_B()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}


// Called when the game starts or when spawned
void ABounceActor_B::BeginPlay()
{
	Super::BeginPlay();
	SetActorRotation(FRotator(0, 50, 0));
	GetWorld()->GetTimerManager().SetTimer(TH_ExplodeTimer,this, &ABounceActor_B::Explode, ExplodeTime, false);
}

// Called every frame
void ABounceActor_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetActorRotation(FRotator(GetActorRotation().Pitch, 50, GetActorRotation().Roll));

}

void ABounceActor_B::Explode()
{
	UGameplayStatics::ApplyRadialDamage(GetWorld(), 0, GetActorLocation(), 500.f, BP_DamageType, {}, this);
	Destroy();
}
