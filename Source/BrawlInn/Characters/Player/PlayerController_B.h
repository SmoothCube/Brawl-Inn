// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Characters/Player/PlayerInfo.h"
#include "PlayerController_B.generated.h"

class UHealthComponent_B;
class APlayerCharacter_B;
class ARespawnPawn_B;
class UCharacterSelectionComponent_B;

UCLASS()
class BRAWLINN_API APlayerController_B : public APlayerController
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

	// ********** Respawn **********
	ARespawnPawn_B* RespawnPawn = nullptr;

	bool bCanRespawn = true;
	
	void Respawn();
	void TryRespawn(float RespawnDelay);

	virtual void OnUnPossess() override;
	
	UPROPERTY(EditAnywhere, Category="Variables")
	bool bVibrateControllers = false;

	//virtual void TakeDamage_Implementation(int DamageAmount) override;



	// ********** Debug **********
	void DEBUG_TEST01();
	void DEBUG_Spawn();
	void DEBUG_Despawn();

	// ********** Input **********
private:
	void MoveUp(float Value);
	void MoveRight(float Value);
	
	void PickupButtonPressed();
	void PickupButtonRepeat();
	void PunchButtonPressed();
	void PunchButtonReleased();
	void BreakFreeButtonPressed();
	void DashButtonPressed();

	// ********** Various **********
	FTimerHandle TH_RespawnTimer;

public:
	FPlayerInfo PlayerInfo;
};
