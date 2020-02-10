// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameCamera_B.generated.h"

class USpringArmComponent;
class UCameraComponent;
class APlayerCharacter_B;
class ATriggerBox;

UCLASS()
class BRAWLINN_API AGameCamera_B : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameCamera_B();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Scene = nullptr;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm = nullptr;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera = nullptr;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float SmallestSpringArmLength = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float LargestSpringArmLength = 2300.f;

	TArray<AActor*> ActorsToTrack;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float BorderWidth = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float MaxCameraHeight = 100000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float MinCameraHeight = -100000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	float LerpAlpha = 0.5;
private:

	void LerpCameraLocation(FVector LerpLoc);

	void UpdateCamera();

	void SetSpringArmLength(float distanceToFurthestPlayer);

	UFUNCTION()
	void OnTrackingBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void OnTrackingBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	ATriggerBox* TrackingBox = nullptr;

};
