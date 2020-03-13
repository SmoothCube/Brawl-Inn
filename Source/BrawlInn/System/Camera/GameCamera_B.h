// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConvexVolume.h"
#include "GameCamera_B.generated.h"


class USpringArmComponent;
class UCameraComponent;
class APlayerCharacter_B;
class ULocalPlayer;
class ATriggerBox;

UCLASS()
class BRAWLINN_API AGameCamera_B : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameCamera_B();


	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComp = nullptr;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm = nullptr;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera = nullptr;


	// ********** AActor **********
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
		// ********** Frustum stuff **********
	ULocalPlayer* LocalPlayer;

	// ********** Camera Movement **********
private:
	void UpdateCameraPosition(FConvexVolume& scene);

	void LerpCameraLocation(FVector LerpLoc);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float MaxCameraHeight = 100000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float MinCameraHeight = -100000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float LerpAlpha = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	FVector CameraOffset = FVector(0,0,0);

	// ********** Zoom **********
private:
	void SetSpringArmLength(FConvexVolume& scene);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float SmallestSpringArmLength = 750.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float LargestSpringArmLength = 3000.f;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		float BorderWidth = 1000.f;

private:
	float CameraZoom = 0.f;

	// ********** Zoom Rotation **********
	void SetSpringArmPitch();

	float StartPitch;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	float LowestRotAdd = -0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	float HighestRotAdd = 15.f;


	// ********** Tracking **********
private:
	UFUNCTION()
	void OnTrackingBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void OnTrackingBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY()
	ATriggerBox* TrackingBox = nullptr;

public:
	UPROPERTY()
	TArray<AActor*> ActorsToTrack;

};
