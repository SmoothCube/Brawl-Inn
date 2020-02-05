// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter_B.h"
#include "Components/StaticMeshComponent.h"

#include "Characters/Player/PlayerController_B.h"
#include "Components/HealthComponent_B.h"
#include "UI/Game/HealthWidget_B.h"

APlayerCharacter_B::APlayerCharacter_B()
{
	DirectionIndicatorPlane = CreateDefaultSubobject<UStaticMeshComponent>("Direction Indicator Plane");
	DirectionIndicatorPlane->SetupAttachment(RootComponent);
	DirectionIndicatorPlane->SetRelativeLocation(FVector(20, 0, -70));
	DirectionIndicatorPlane->SetRelativeRotation(FRotator(0, 90, 0));
	DirectionIndicatorPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DirectionIndicatorPlane->SetRelativeScale3D(FVector(3.327123, 3.327123, 1));
}

void APlayerCharacter_B::BeginPlay()
{
	Super::BeginPlay();

	//Sends the icons to to the health widget.
	//GetPlayerController_B()->HealthComponent->HealthWidget->PostInitialize();

}
