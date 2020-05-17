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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	// ********** Camera Movement **********
private:
	void UpdateCameraPosition(FConvexVolume& scene);

	void LerpCameraLocation(FVector LerpLoc);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxCameraHeight = 100000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MinCameraHeight = -100000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LerpAlpha = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraMoveSpeed = 0.03f;

	// ********** Zoom **********
private:
	void SetSpringArmLength(FConvexVolume& scene);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SmallestSpringArmLength = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LargestSpringArmLength = 4500.f;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LeftBorderWidth = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RightBorderWidth = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float UpBorderWidth = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DownBorderWidth = 750.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CameraZoomSpeed = 0.05f;
private:
	float CameraZoom = 0.f;

	// ********** Zoom Rotation **********
	void SetSpringArmPitch();

	float StartPitch;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LowestRotAdd = -5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HighestRotAdd = 20.f;

	// ********** Tracking **********
private:
	void EndFocus(AActor* OtherActor);

	UFUNCTION()
	void OnTrackingBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void OnTrackingBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY()
	ATriggerBox* TrackingBox = nullptr;
	
	FTimerHandle TH_EndFocusTimer;

	float EndFocusTime = 1.f;
public:
	UPROPERTY(EditAnywhere)
	TArray<AActor*> ActorsToTrack;

};
