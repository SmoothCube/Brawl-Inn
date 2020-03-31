// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/PlayerController_B.h"
#include "GamePlayerController_B.generated.h"

class AMainGameMode_B;
class ARespawnPawn_B;
class UColoredTextBlock_B;

UCLASS()
class BRAWLINN_API AGamePlayerController_B : public APlayerController_B
{
	GENERATED_BODY()

protected:

	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void OnPossess(APawn* InPawn) override;

	void DPadUpPressed() override;

	void DPadDownPressed() override;

	void FaceButtonTopPressed() override;
	void FaceButtonTopReleased() override;

	void FaceButtonRightPressed() override;

	void FaceButtonBottomPressed() override;

	void FaceButtonBottomRepeat() override;

	void FaceButtonLeftPressed() override;
	void FaceButtonLeftReleased() override;

	void SpecialRightPressed() override;

	void LeftShoulderPressed() override;

	void RightShoulderPressed() override;
	void RightShoulderReleased() override;

	void LeftTriggerPressed() override;
	void LeftTriggerRepeat() override;

	void RightTriggerPressed() override;
	void RightTriggerReleased() override;



	void LeftStickXAxis(float Value) override;

	void LeftStickYAxis(float Value) override;
	
	void TryPauseGame();
	///Returns true if the player character is being held
	bool TryBreakFree();

	bool TryThrow();

	bool TryStartPunchCharge();

	bool TryStartThrowCharge();

	bool TryEndPunchCharge();

	bool TryPunch();

	void TryDash();

	void TryPickup();


	// ********** Respawn **********
public:
	ARespawnPawn_B* RespawnPawn = nullptr;
protected:
	bool bCanRespawn = true;

	void Respawn() const;

	FTimerHandle TH_RespawnTimer;

public:
	void TryRespawn(const float ReSpawnDelay);

	void SetScoreTextBlock(UColoredTextBlock_B* TextBlock);
protected:

	UPROPERTY()
		UColoredTextBlock_B* ScoreTextBlock = nullptr;

	UPROPERTY(BlueprintReadOnly)
	AMainGameMode_B* MainGameMode = nullptr;

	// ********** EditorOnly **********
	void Debug_Spawn() const;
	void Debug_DeSpawn();
};
