// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "System/Structs/ScoreValues.h"
#include "HealthWidget_B.generated.h"

class UImage;
class UTextBlock;
class APlayerCharacter_B;

UCLASS()
class BRAWLINN_API UHealthWidget_B : public UUserWidget
{
	GENERATED_BODY()
protected:

	virtual void NativeConstruct() override;
	virtual bool Initialize() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* PlayerIcon;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* ScoreValueText;

public:

	UFUNCTION(BlueprintImplementableEvent)
		void PostInitialize(APlayerCharacter_B* Character);

	void UpdateScoreValues(FScoreValues ScoreValues) const;

};
