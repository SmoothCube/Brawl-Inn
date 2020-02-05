// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverlay_B.generated.h"

class UHealthWidget_B;
class UGameInstance_B;

UCLASS()
class BRAWLINN_API UGameOverlay_B : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;
	
	UPROPERTY(meta = (BindWidget))
	UHealthWidget_B* Player0;

	UPROPERTY(meta = (BindWidget))
	UHealthWidget_B* Player1;

	UPROPERTY(meta = (BindWidget))
	UHealthWidget_B* Player2;

	UPROPERTY(meta = (BindWidget))
	UHealthWidget_B* Player3;

	UPROPERTY()
	TArray<UHealthWidget_B*> HealthWidgets;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UHealthWidget_B> BP_HealthWidget;

	UPROPERTY()
	UGameInstance_B* GameInstance = nullptr;

	UFUNCTION()
	void ChangeHealthWidgetVisibility();

};
