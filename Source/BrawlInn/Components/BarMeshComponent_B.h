// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "BarMeshComponent_B.generated.h"

DECLARE_MULTICAST_DELEGATE(FItemDetached_NOPARAM);

UCLASS()
class BRAWLINN_API UBarMeshComponent_B : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	FItemDetached_NOPARAM OnItemDetach;

protected:
	virtual void OnChildDetached(USceneComponent* ChildComponent) override;
};
