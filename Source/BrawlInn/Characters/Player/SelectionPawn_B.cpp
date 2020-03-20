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
	SetActorRotation(FRotator(0, 25, -25));
}

UPaperSpriteComponent* ASelectionPawn_B::GetSpriteIcon() const
{
	return SpriteIcon;
}
