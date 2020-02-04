// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthWidget_B.generated.h"

class UProgressBar;
class UHorizontalBox;

UCLASS()
class BRAWLINN_API UHealthWidget_B : public UUserWidget
{
	GENERATED_BODY()
protected:

	virtual void NativeConstruct() override;
	virtual bool Initialize() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UHorizontalBox* BarrelArea;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UProgressBar* HealthProgressBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush brush;

public:

	UFUNCTION()
	void UpdateHealthAmount(int Amount);

};
