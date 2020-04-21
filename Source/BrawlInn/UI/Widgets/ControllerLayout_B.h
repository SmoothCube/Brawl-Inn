// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ControllerLayout_B.generated.h"

class UButton_B;

UCLASS()
class BRAWLINN_API UControllerLayout_B : public UUserWidget
{
	GENERATED_BODY()

		void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
public:

	UButton_B* GetBackButton() const;

protected:
	UPROPERTY(meta = (BindWidget))
		UButton_B* BackButton;

};
