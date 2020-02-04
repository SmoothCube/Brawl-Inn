// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter_B.h"
#include "Components/StaticMeshComponent.h"

APlayerCharacter_B::APlayerCharacter_B()
{
	DirectionIndicatorPlane = CreateDefaultSubobject<UStaticMeshComponent>("Direction Indicator Plane");
	DirectionIndicatorPlane->SetupAttachment(RootComponent);
	DirectionIndicatorPlane->SetRelativeLocation(FVector(20, 0, -70));
	DirectionIndicatorPlane->SetRelativeRotation(FRotator(0, 90, 0));
	DirectionIndicatorPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DirectionIndicatorPlane->SetRelativeScale3D(FVector(3.327123, 3.327123, 1));
}