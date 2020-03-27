// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "BrawlInn.h"
#include "System/GameModes/MenuGameMode_B.h"

APlayerController_B::APlayerController_B()
{
}

void APlayerController_B::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameOnly());
	bAutoManageActiveCameraTarget = false;
}

void APlayerController_B::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent != nullptr)
	{
		// DPad
		InputComponent->BindAction("DPadUp", IE_Pressed, this, &APlayerController_B::DPadUpPressed);
		InputComponent->BindAction("DPadUp", IE_Released, this, &APlayerController_B::DPadUpReleased);
		InputComponent->BindAction("DPadUp", IE_Repeat, this, &APlayerController_B::DPadUpRepeat);

		InputComponent->BindAction("DPadRight", IE_Pressed, this, &APlayerController_B::DPadRightPressed);
		InputComponent->BindAction("DPadRight", IE_Released, this, &APlayerController_B::DPadRightReleased);
		InputComponent->BindAction("DPadRight", IE_Repeat, this, &APlayerController_B::DPadRightRepeat);

		InputComponent->BindAction("DPadDown", IE_Pressed, this, &APlayerController_B::DPadDownPressed);
		InputComponent->BindAction("DPadDown", IE_Released, this, &APlayerController_B::DPadDownReleased);
		InputComponent->BindAction("DPadDown", IE_Repeat, this, &APlayerController_B::DPadDownRepeat);

		InputComponent->BindAction("DPadLeft", IE_Pressed, this, &APlayerController_B::DPadLeftPressed);
		InputComponent->BindAction("DPadLeft", IE_Released, this, &APlayerController_B::DPadLeftReleased);
		InputComponent->BindAction("DPadLeft", IE_Repeat, this, &APlayerController_B::DPadLeftRepeat);

		// Face buttons
		InputComponent->BindAction("FaceButtonTop", IE_Pressed, this, &APlayerController_B::FaceButtonTopPressed);
		InputComponent->BindAction("FaceButtonTop", IE_Released, this, &APlayerController_B::FaceButtonTopReleased);
		InputComponent->BindAction("FaceButtonTop", IE_Repeat, this, &APlayerController_B::FaceButtonTopRepeat);

		InputComponent->BindAction("FaceButtonRight", IE_Pressed, this, &APlayerController_B::FaceButtonRightPressed);
		InputComponent->BindAction("FaceButtonRight", IE_Released, this, &APlayerController_B::FaceButtonRightReleased);
		InputComponent->BindAction("FaceButtonRight", IE_Repeat, this, &APlayerController_B::FaceButtonRightRepeat);

		InputComponent->BindAction("FaceButtonBottom", IE_Pressed, this, &APlayerController_B::FaceButtonBottomPressed);
		InputComponent->BindAction("FaceButtonBottom", IE_Released, this, &APlayerController_B::FaceButtonBottomReleased);
		InputComponent->BindAction("FaceButtonBottom", IE_Repeat, this, &APlayerController_B::FaceButtonBottomRepeat);

		InputComponent->BindAction("FaceButtonLeft", IE_Pressed, this, &APlayerController_B::FaceButtonLeftPressed);
		InputComponent->BindAction("FaceButtonLeft", IE_Released, this, &APlayerController_B::FaceButtonLeftReleased);
		InputComponent->BindAction("FaceButtonLeft", IE_Repeat, this, &APlayerController_B::FaceButtonLeftRepeat);

		// Triggers/Shoulder
		InputComponent->BindAction("LeftShoulder", IE_Pressed, this, &APlayerController_B::LeftShoulderPressed);
		InputComponent->BindAction("LeftShoulder", IE_Released, this, &APlayerController_B::LeftShoulderReleased);
		InputComponent->BindAction("LeftShoulder", IE_Repeat, this, &APlayerController_B::LeftShoulderRepeat);
		
		InputComponent->BindAction("LeftTrigger", IE_Pressed, this, &APlayerController_B::LeftTriggerPressed);
		InputComponent->BindAction("LeftTrigger", IE_Released, this, &APlayerController_B::LeftTriggerReleased);
		InputComponent->BindAction("LeftTrigger", IE_Repeat, this, &APlayerController_B::LeftTriggerRepeat);

		InputComponent->BindAction("RightShoulder", IE_Pressed, this, &APlayerController_B::RightShoulderPressed);
		InputComponent->BindAction("RightShoulder", IE_Released, this, &APlayerController_B::RightShoulderReleased);
		InputComponent->BindAction("RightShoulder", IE_Repeat, this, &APlayerController_B::RightShoulderRepeat);

		InputComponent->BindAction("RightTrigger", IE_Pressed, this, &APlayerController_B::RightTriggerPressed);
		InputComponent->BindAction("RightTrigger", IE_Released, this, &APlayerController_B::RightTriggerReleased);
		InputComponent->BindAction("RightTrigger", IE_Repeat, this, &APlayerController_B::RightTriggerRepeat);

		// Special
		InputComponent->BindAction("SpecialLeft", IE_Pressed, this, &APlayerController_B::SpecialLeftPressed);
		InputComponent->BindAction("SpecialLeft", IE_Released, this, &APlayerController_B::SpecialLeftReleased);
		InputComponent->BindAction("SpecialLeft", IE_Repeat, this, &APlayerController_B::SpecialLeftRepeat);

		InputComponent->BindAction("SpecialRight", IE_Pressed, this, &APlayerController_B::SpecialRightPressed);
		InputComponent->BindAction("SpecialRight", IE_Released, this, &APlayerController_B::SpecialRightReleased);
		InputComponent->BindAction("SpecialRight", IE_Repeat, this, &APlayerController_B::SpecialRightRepeat);

		// Sticks
		InputComponent->BindAxis("LeftStickX", this, &APlayerController_B::LeftStickXAxis);
		InputComponent->BindAxis("LeftStickY", this, &APlayerController_B::LeftStickYAxis);

		InputComponent->BindAction("LeftStickRight", IE_Pressed, this, &APlayerController_B::LeftStickRightPressed);
		InputComponent->BindAction("LeftStickRight", IE_Released, this, &APlayerController_B::LeftStickRightReleased);
		InputComponent->BindAction("LeftStickRight", IE_Repeat, this, &APlayerController_B::LeftStickRightRepeat);

		InputComponent->BindAction("LeftStickLeft", IE_Pressed, this, &APlayerController_B::LeftStickLeftPressed);
		InputComponent->BindAction("LeftStickLeft", IE_Released, this, &APlayerController_B::LeftStickLeftReleased);
		InputComponent->BindAction("LeftStickLeft", IE_Repeat, this, &APlayerController_B::LeftStickLeftRepeat);

		
		InputComponent->BindAxis("RightStickX", this, &APlayerController_B::RightStickXAxis);
		InputComponent->BindAxis("RightStickY", this, &APlayerController_B::RightStickYAxis);
	}
}

bool APlayerController_B::HasValidCharacter() const
{
	if (PlayerCharacter)
		return true;
	return false;
}

const FPlayerInfo& APlayerController_B::GetPlayerInfo() const
{
	return PlayerInfo;
}

void APlayerController_B::SetPlayerInfo(const FPlayerInfo& Info)
{
	PlayerInfo = Info;
}

void APlayerController_B::PlayControllerVibration(float Strength, float Duration, bool bAffectsLeftLarge, bool bAffectsLeftSmall, bool bAffectsRightLarge, bool bAffectsRightSmall, EDynamicForceFeedbackAction::Type Action)
{
	if (bVibrateControllers)
		PlayDynamicForceFeedback(Strength, Duration, bAffectsLeftLarge, bAffectsLeftSmall, bAffectsRightLarge, bAffectsRightSmall, Action);

}

APlayerCharacter_B* APlayerController_B::GetPlayerCharacter() const
{
	return PlayerCharacter;
}

void APlayerController_B::SetPlayerCharacter(APlayerCharacter_B* CharacterIn)
{
	PlayerCharacter = CharacterIn;
}

void APlayerController_B::FellOutOfWorld(const UDamageType& DmgType)
{

}

void APlayerController_B::OnUnPossess()
{
	Super::OnUnPossess();
	PlayerCharacter = nullptr;
}

