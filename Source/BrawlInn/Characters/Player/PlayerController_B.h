// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Characters/Player/PlayerInfo.h"
#include "PlayerController_B.generated.h"

class APlayerCharacter_B;

UCLASS()
class BRAWLINN_API APlayerController_B : public APlayerController
{
	GENERATED_BODY()

protected:
	APlayerController_B();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void FellOutOfWorld(const class UDamageType& DmgType) override;
	virtual void OnUnPossess() override;

	// ********** KeyBindings **********

	virtual void DPadUpPressed() {}
	virtual void DPadUpReleased() {}
	virtual void DPadUpRepeat() {}

	virtual void DPadRightPressed() {}
	virtual void DPadRightReleased() {}
	virtual void DPadRightRepeat() {}

	virtual void DPadDownPressed() {}
	virtual void DPadDownReleased() {}
	virtual void DPadDownRepeat() {}

	virtual void DPadLeftPressed() {}
	virtual void DPadLeftReleased() {}
	virtual void DPadLeftRepeat() {}

	virtual void FaceButtonTopPressed() {}
	virtual void FaceButtonTopReleased() {}
	virtual void FaceButtonTopRepeat() {}

	virtual void FaceButtonRightPressed() {}
	virtual void FaceButtonRightReleased() {}
	virtual void FaceButtonRightRepeat() {}

	virtual void FaceButtonBottomPressed() {}
	virtual void FaceButtonBottomReleased() {}
	virtual void FaceButtonBottomRepeat() {}

	virtual void FaceButtonLeftPressed() {}
	virtual void FaceButtonLeftReleased() {}
	virtual void FaceButtonLeftRepeat() {}

	virtual void LeftShoulderPressed() {}
	virtual void LeftShoulderReleased() {}
	virtual void LeftShoulderRepeat() {}

	virtual void LeftTriggerPressed() {}
	virtual void LeftTriggerReleased() {}
	virtual void LeftTriggerRepeat() {}

	virtual void RightShoulderPressed() {}
	virtual void RightShoulderReleased() {}
	virtual void RightShoulderRepeat() {}

	virtual void RightTriggerPressed() {}
	virtual void RightTriggerReleased() {}
	virtual void RightTriggerRepeat() {}

	virtual void SpecialLeftPressed() {}
	virtual void SpecialLeftReleased() {}
	virtual void SpecialLeftRepeat() {}

	virtual void SpecialRightPressed() {}
	virtual void SpecialRightReleased() {}
	virtual void SpecialRightRepeat() {}

	virtual void LeftStickXAxis(float Value) {}
	virtual void LeftStickYAxis(float Value) {}
	
	virtual void LeftStickRightPressed() {}
	virtual void LeftStickRightReleased() {}
	virtual void LeftStickRightRepeat() {}
	
	virtual void LeftStickLeftPressed() {}
	virtual void LeftStickLeftReleased() {}
	virtual void LeftStickLeftRepeat() {}
	
	virtual void RightStickXAxis(float Value) {}
	virtual void RightStickYAxis(float Value) {}

	// ********** Various **********
public:
	UFUNCTION()
	void PlayControllerVibration(float Strength, float Duration, bool bAffectsLeftLarge, bool bAffectsLeftSmall, bool bAffectsRightLarge, bool bAffectsRightSmall, EDynamicForceFeedbackAction::Type Action = EDynamicForceFeedbackAction::Start);

	APlayerCharacter_B* GetPlayerCharacter() const;

	void SetPlayerCharacter(APlayerCharacter_B* Character);
	
	bool HasValidCharacter() const;

	const FPlayerInfo& GetPlayerInfo() const;

	void SetPlayerInfo(const FPlayerInfo& Info);
	
protected:

	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter_B* PlayerCharacter = nullptr;

	UPROPERTY(EditAnywhere)
		bool bVibrateControllers = false;

	FPlayerInfo PlayerInfo;
};
