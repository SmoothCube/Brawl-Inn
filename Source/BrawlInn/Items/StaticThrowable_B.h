// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Throwable_B.h"
#include "StaticThrowable_B.generated.h"

class UStaticMeshComponent;


UCLASS()
class BRAWLINN_API AStaticThrowable_B : public AThrowable_B
{
	GENERATED_BODY()
	
public:

	AStaticThrowable_B();

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;

	virtual AThrowable_B* PickedUp(APlayerCharacter_B* Owner) override;
	virtual void Dropped() override;
	virtual UMeshComponent* GetMesh() override;

};
