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

	void PlayControllerVibration(float Strength,float Duration,bool bAffectsLeftLarge,bool bAffectsLeftSmall,bool bAffectsRightLarge,bool bAffectsRightSmall, EDynamicForceFeedbackAction::Type Action= EDynamicForceFeedbackAction::Start);

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthComponent_B* HealthComponent;

	APlayerCharacter_B* PlayerCharacter = nullptr;

	UFUNCTION()
		void KillPlayerCharacter();
	
	UPROPERTY(EditAnywhere, Category="Variables")
	bool bVibrateControllers = false;


private:

	void MoveUp(float Value);
	void MoveRight(float Value);
	void RotateX(float Value);
	void RotateY(float Value);

	void PickupButtonPressed();
	void PickupButtonRepeat();
	void PunchButtonPressed();
};
