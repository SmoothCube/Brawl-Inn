// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AudioSettingsWidget_B.generated.h"

class UFont;
class USettingsWidget_B;
class UGameInstance_B;
class UTextBlock;
class USlider_B;
class UButton_B;

UCLASS()
class BRAWLINN_API UAudioSettingsWidget_B : public UUserWidget
{
	GENERATED_BODY()
protected:

	void NativeOnInitialized() override;

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ********** Sliders **********

	UFUNCTION()
		void OnSfxValueChanged(float Value);

	UFUNCTION()
		void OnMasterValueChanged(float Value);

	UFUNCTION()
		void OnMusicValueChanged(float Value);

	// ********** Widgets **********

public:
	UPROPERTY(meta = (BindWidget))
		USlider_B* MasterSlider;

	UPROPERTY(meta = (BindWidget))
		USlider_B* MusicSlider;

	UPROPERTY(meta = (BindWidget))
		USlider_B* SfxSlider;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* MasterText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* MusicText;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* SfxText;


	// ********** Misc. **********

	UPROPERTY(EditDefaultsOnly)
		UFont* TextFont;

	UPROPERTY()
		UGameInstance_B* GameInstance = nullptr;

	UPROPERTY()
		TArray<UWidget*> UIElementsWithInterface;
};
