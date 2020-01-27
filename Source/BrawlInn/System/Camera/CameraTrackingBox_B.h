// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "CameraTrackingBox_B.generated.h"

/**
 * 
 */

class APlayerController_B;

UCLASS()
class BRAWLINN_API ACameraTrackingBox_B : public ATriggerBox
{
	GENERATED_BODY()

	ACameraTrackingBox_B();
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);
public: 
	TArray<APlayerController_B*> PlayerControllers;
};
