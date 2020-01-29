// Fill out your copyright notice in the Description page of Project Settings.


#include "AIController_B.h"
#include "BrawlInn.h"
#include "Components/HealthComponent_B.h"

AAIController_B::AAIController_B()
{
	HealthComponent = CreateDefaultSubobject<UHealthComponent_B>("Health Component");
}

void AAIController_B::TakeOneDamage_Implementation()
{
	HealthComponent->TakeDamage(1);
}