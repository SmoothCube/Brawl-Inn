// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Throwable_B.h"
#include "DestructibleThrowable_B.generated.h"

class UDestructibleComponent;

UCLASS()
class BRAWLINN_API ADestructibleThrowable_B : public AThrowable_B
{
	GENERATED_BODY()
	
public:

	ADestructibleThrowable_B();

	UPROPERTY(VisibleAnywhere)
		UDestructibleComponent* DestructibleMesh;

	virtual AThrowable_B* PickedUp(APlayerCharacter_B* Owner) override;
	virtual void Dropped() override;
	virtual UMeshComponent* GetMesh() override;
};
