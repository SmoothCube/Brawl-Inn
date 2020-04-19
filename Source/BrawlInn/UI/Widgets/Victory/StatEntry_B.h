// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatEntry_B.generated.h"

class UTextBlock;

UCLASS()
class BRAWLINN_API UStatEntry_B : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite , meta = (BindWidget))
		UTextBlock* Number;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* Text;

};
