// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/GameModes/GameMode_B.h"
#include "MainGameMode_B.generated.h"

class AGameCamera_B;
class UVictoryScreenWidget_B;
class USceneComponent; 
class APlayerCharacter;
class UPauseMenu_B;
class UGameOverlay_B;

DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerWin, APlayerController_B*);

UCLASS()
class BRAWLINN_API AMainGameMode_B : public AGameMode_B
{
	GENERATED_BODY()

protected:

	// ** Overridden functions **

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void UpdateViewTarget(APlayerController_B* PlayerController) override;

public:
	void PauseGame(APlayerController_B* ControllerThatPaused);
	void ResumeGame();
	APlayerController_B* PlayerControllerThatPaused = nullptr;

	FPlayerWin OnPlayerWin;

	void AddCameraFocusPoint(USceneComponent* FocusComponent);
	void RemoveCameraFocusPoint(USceneComponent* FocusComponent);
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		TSubclassOf<AGameCamera_B> BP_GameCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|UserWidgets")
		TArray<APlayerCharacter> BP_PlayerCharacters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|UserWidgets")
		TSubclassOf<UPauseMenu_B> BP_PauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|UserWidgets")
		TSubclassOf<UVictoryScreenWidget_B> BP_VictoryScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|UserWidgets")
		TSubclassOf<UGameOverlay_B> BP_GameOverlay;

	UPROPERTY(BlueprintReadWrite)
	UPauseMenu_B* PauseMenuWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* Birds;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* River;

private:
	AGameCamera_B* GameCamera = nullptr;


};
