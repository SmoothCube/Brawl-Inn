// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "System/Structs/ScoreValues.h"
#include "LeaderFollower_B.generated.h"


class APlayerController_B;
class AGamePlayerController_B;
class UStaticMeshComponent;
UCLASS()
class BRAWLINN_API ALeaderFollower_B : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALeaderFollower_B();
	// ********** Overrides **********
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;



	// ********** Components **********
protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
	
	void ScoreUpdated();

	UPROPERTY()
	APlayerController_B* LeadingPlayerController = nullptr;

	UPROPERTY()
		APlayerController_B* NextControllerToFollow = nullptr;
	

	// ********** Positioning **********
	UPROPERTY(EditAnywhere)
	float LerpAlpha = 0.7;

	UPROPERTY(EditAnywhere)
	FVector Offset = FVector(25, 0, 135);
public:

	// ********** Time Tracking **********
private:
	float CurrentCrownTime = 0;
	
public:
	// ********** Bobbing **********
	float GetBobbingHeight(float Time);

	UPROPERTY(EditAnywhere)
	float BobFrequency = 4.f;

	UPROPERTY(EditAnywhere)
	float BobAmplitude = 10.f;

	// ********** Animations **********

	UFUNCTION(BlueprintImplementableEvent)
		void Appear();

	UFUNCTION(BlueprintImplementableEvent)
		void Dissapear();

	UFUNCTION(BlueprintCallable)
		void AnimationTimelineFinished(bool bAppeared);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AnimationRotationIncrease = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AnimationZOffset = 0.f;

	// ********** Misc **********
	UPROPERTY(EditAnywhere)
	float RotationSpeed = 0.88f;
};