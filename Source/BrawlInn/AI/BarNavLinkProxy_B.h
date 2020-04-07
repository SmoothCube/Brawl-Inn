// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "BarNavLinkProxy_B.generated.h"

class UStaticMeshComponent;

UCLASS()
class BRAWLINN_API ABarNavLinkProxy_B : public ANavLinkProxy
{
	GENERATED_BODY()
protected:
	
	ABarNavLinkProxy_B();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite)
	bool bDoorIsOpen = false;
};
