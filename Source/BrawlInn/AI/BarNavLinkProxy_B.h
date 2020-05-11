// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "BarNavLinkProxy_B.generated.h"

UCLASS()
class BRAWLINN_API ABarNavLinkProxy_B : public ANavLinkProxy
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void PostLevelLoad();

};
