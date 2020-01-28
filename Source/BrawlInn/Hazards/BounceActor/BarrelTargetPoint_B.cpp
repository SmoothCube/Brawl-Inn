// Fill out your copyright notice in the Description page of Project Settings.


#include "BarrelTargetPoint_B.h"
#include "Components/DecalComponent.h"

ABarrelTargetPoint_B::ABarrelTargetPoint_B()
{
	Decal = CreateDefaultSubobject<UDecalComponent>("Decal");
	Decal->AddLocalRotation(FRotator(90, 0, 0));

	RootComponent->SetVisibility(true, true);
}
