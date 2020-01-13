// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerController_B.generated.h"

class UHealthComponent_B;
class APlayerCharacter_B;

UCLASS()
class BRAWLINN_API APlayerController_B : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerController_B();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	bool HasValidCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UHealthComponent_B* HealthComponent;


	APlayerCharacter_B* PlayerCharacter = nullptr;

	UFUNCTION()
		void KillPlayerCharacter();
private:

	void MoveUp(float Value);
	void MoveRight(float Value);
	void RotateX(float Value);
	void RotateY(float Value);

	void PickupButtonPressed();
	void PickupButtonRepeat();
	void PunchButtonPressed();
};
