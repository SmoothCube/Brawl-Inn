// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/GameMode_B.h"
#include "MenuGameMode_B.generated.h"

class ACameraActor;

UCLASS()
class BRAWLINN_API AMenuGameMode_B : public AGameMode_B
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACameraActor* Camera = nullptr;

	UFUNCTION()
	void UpdateViewTarget(APlayerController_B* PlayerController);

	UFUNCTION(BlueprintCallable)
	void UpdateViewTargets();
};
