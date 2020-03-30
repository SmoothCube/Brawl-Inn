// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AudioSettingsWidget_B.generated.h"

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
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		USlider_B* MasterSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		USlider_B* MusicSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		USlider_B* SfxSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* SfxValueText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* MasterValueText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* MusicValueText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton_B* InvisibleButton;
	
	// ********** Misc. **********
	
	UPROPERTY()
		UGameInstance_B* GameInstance = nullptr;

	UPROPERTY()
		TArray<UWidget*> UIElementsWithInterface;
};
