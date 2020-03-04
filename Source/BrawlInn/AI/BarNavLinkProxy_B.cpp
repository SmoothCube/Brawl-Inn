// Fill out your copyright notice in the Description page of Project Settings.


#include "BarNavLinkProxy_B.h"
#include "Components/StaticMeshComponent.h"

ABarNavLinkProxy_B::ABarNavLinkProxy_B()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(GetRootComponent());

}

void ABarNavLinkProxy_B::BeginPlay()
{
	Super::BeginPlay();
}
