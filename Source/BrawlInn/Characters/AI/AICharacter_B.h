// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Character_B.h"
#include "AICharacter_B.generated.h"

class AItem_B;

UCLASS()
class BRAWLINN_API AAICharacter_B : public ACharacter_B
{
	GENERATED_BODY()

		virtual void BeginPlay() override;

	virtual void FellOutOfWorld(const UDamageType& DmgType) override;
	
	void Deliver(AItem_B* ItemToDeliver, AAICharacter_B* CharacterToDeliver);

public:
	AItem_B* GetItemDelivered() const;
	
	UPROPERTY()
	AItem_B* ItemDelivered = nullptr;
	
	FTransform StartTransform;
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Variables")
	bool bShouldRespawn = false;
};
