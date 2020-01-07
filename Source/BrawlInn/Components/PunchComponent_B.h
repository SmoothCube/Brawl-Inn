// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PunchComponent_B.generated.h"

class APlayerCharacter_B;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRAWLINN_API UPunchComponent_B : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPunchComponent_B();

	UFUNCTION(BlueprintCallable)
	bool GetIsPunching();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	APlayerCharacter_B* Player = nullptr;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void PunchStart();

	UFUNCTION(BlueprintCallable)
		void PunchEnd();
		
	bool bIsPunching = false;
};
