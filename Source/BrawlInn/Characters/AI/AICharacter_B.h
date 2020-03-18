// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Character_B.h"
#include "AICharacter_B.generated.h"

class UPawnSensingComponent;
class AItem_B;

UCLASS()
class BRAWLINN_API AAICharacter_B : public ACharacter_B
{
	GENERATED_BODY()
protected:

	AAICharacter_B();

	// ********** AActor **********

	virtual void BeginPlay() override;

	virtual void FellOutOfWorld(const UDamageType& DmgType) override;


	// ********** Components **********
public:
	UPawnSensingComponent* GetPawnSensingComponent() const;
protected:
	
	UPROPERTY(VisibleAnywhere)
		UPawnSensingComponent* PawnSensingComponent;

	// ********** Deliver **********
public:
	AItem_B* GetItemDelivered() const;

	void SetItemDelivered(AItem_B* Item);

protected:
	void Deliver(AItem_B* ItemToDeliver, AAICharacter_B* CharacterToDeliver);

	UPROPERTY()
		AItem_B* ItemDelivered = nullptr;

	// ********** Misc. **********
public:

	UFUNCTION(BlueprintCallable)
		FTransform GetStartTransform() const;

protected:

	FTransform StartTransform;

	UPROPERTY(EditDefaultsOnly, Category = "Variables")
		bool bShouldRespawn = false;
};
