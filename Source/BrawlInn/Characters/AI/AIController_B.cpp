// Fill out your copyright notice in the Description page of Project Settings.


#include "AIController_B.h"
#include "BrawlInn.h"
#include "AI/PathFollowingComponent_B.h"

AAIController_B::AAIController_B(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UPathFollowingComponent_B>(TEXT("PathFollowingComponent")))
{
}

void AAIController_B::TakeDamage_Implementation(int DamageAmount)
{
}
