// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthWidget_B.generated.h"

class UProgressBar;
class UVerticalBox;
class UImage;

UCLASS()
class BRAWLINN_API UHealthWidget_B : public UUserWidget
{
	GENERATED_BODY()
protected:

	virtual void NativeConstruct() override;
	virtual bool Initialize() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UVerticalBox* RespawnCharges;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UProgressBar* HealthProgressBar;

	UPROPERTY()
	TArray<UImage*> Barrels;

public:

	UFUNCTION(BlueprintImplementableEvent)
	void PostInitialize();

	UFUNCTION()
	void UpdateHealthAmount(int Amount);

	UFUNCTION()
		void UpdateRespawnsAmount();

};
