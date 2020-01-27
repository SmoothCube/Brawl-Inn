// Fill out your copyright notice in the Description page of Project Settings.

#include "BarMeshComponent_B.h"

void UBarMeshComponent_B::OnChildDetached(USceneComponent* ChildComponent)
{
	Super::OnChildDetached(ChildComponent);

	OnItemDetach.Broadcast();
}
