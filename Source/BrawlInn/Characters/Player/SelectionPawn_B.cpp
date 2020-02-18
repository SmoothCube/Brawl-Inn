// Fill out your copyright notice in the Description page of Project Settings.

#include "SelectionPawn_B.h"
#include "PaperSpriteComponent.h"

ASelectionPawn_B::ASelectionPawn_B()
{
	PrimaryActorTick.bCanEverTick = false;

	SpriteIcon = CreateDefaultSubobject<UPaperSpriteComponent>("SpriteIcon");
	SetRootComponent(SpriteIcon);
	SpriteIcon->SetRelativeScale3D(FVector(0.2f));

	SpriteIcon->SetGenerateOverlapEvents(false);
	SpriteIcon->SetCollisionProfileName("NoCollision");
}

void ASelectionPawn_B::BeginPlay()
{
	Super::BeginPlay();
	SpriteIcon->SetRelativeRotation(FRotator(0, 90, 0));
}
