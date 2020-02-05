// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "System/Interfaces/ControllerInterface_B.h"
#include "PlayerController_B.generated.h"

class UHealthComponent_B;
class APlayerCharacter_B;
class ARespawnPawn_B;
class UCharacterSelectionComponent_B;

UCLASS()
class BRAWLINN_API APlayerController_B : public APlayerController, public IControllerInterface_B
{
	GENERATED_BODY()

public:
	APlayerController_B();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthComponent_B* HealthComponent;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void TryPauseGame();

	bool HasValidCharacter();

	void PlayControllerVibration(float Strength,float Duration,bool bAffectsLeftLarge,bool bAffectsLeftSmall,bool bAffectsRightLarge,bool bAffectsRightSmall, EDynamicForceFeedbackAction::Type Action= EDynamicForceFeedbackAction::Start);

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;


	APlayerCharacter_B* PlayerCharacter = nullptr;
	ARespawnPawn_B* RespawnPawn = nullptr;


	UFUNCTION()
		void KillPlayerCharacter();
	
	UPROPERTY(EditAnywhere, Category="Variables")
	bool bVibrateControllers = false;

	virtual void TakeDamage_Implementation(int DamageAmount) override;

	// DEBUG
	void DEBUG_TEST01();
	void DEBUG_Spawn();
	void DEBUG_Despawn();

private:

	void MoveUp(float Value);
	void MoveRight(float Value);
	void SelectRight();
	void Unselect();

	void PickupButtonPressed();
	void PickupButtonRepeat();
	void PunchButtonPressed();
	void PunchButtonReleased();
	void BreakFreeButtonPressed();
};
