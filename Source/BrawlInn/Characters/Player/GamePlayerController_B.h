// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/PlayerController_B.h"
#include "GamePlayerController_B.generated.h"

class ARespawnPawn_B;
class UHealthWidget_B;

UCLASS()
class BRAWLINN_API AGamePlayerController_B : public APlayerController_B
{
	GENERATED_BODY()

protected:

	void BeginPlay() override;
	
	void OnPossess(APawn* InPawn) override;

	void DPadUpPressed() override;

	void DPadDownPressed() override;

	void FaceButtonTopPressed() override;
	void FaceButtonTopRepeat() override;

	void FaceButtonRightPressed() override;

	void FaceButtonBottomPressed() override;

	void FaceButtonLeftPressed() override;
	void FaceButtonLeftReleased() override;

	void SpecialRightPressed() override;

	void LeftShoulderPressed() override;
	void TryDash();

	bool TryStartPunchCharge();
	bool TryStartThrowCharge();
	bool TryPunch();
	void RightShoulderPressed() override;

	void LeftTriggerPressed() override;
	void TryPickup();
	void LeftTriggerRepeat() override;

	void RightTriggerPressed() override;
	void PunchOrThrowCharge();
	void RightTriggerReleased() override;

	bool TryThrow();

	void PunchOrThrow();

	void LeftStickXAxis(float Value) override;

	void LeftStickYAxis(float Value) override;

	void TryPauseGame();
	///Returns true if the player character is being held
	bool TryBreakFree();

	// ********** Respawn **********
public:
	ARespawnPawn_B* RespawnPawn = nullptr;
protected:
	bool bCanRespawn = true;

	void Respawn() const;

	FTimerHandle TH_RespawnTimer;

public:
	void TryRespawn(const float ReSpawnDelay);

	void SetHealthWidget(UHealthWidget_B* Widget);
protected:

	UPROPERTY()
		UHealthWidget_B* HealthWidget = nullptr;


	// ********** EditorOnly **********
	void Debug_Spawn() const;
	void Debug_DeSpawn();
};
