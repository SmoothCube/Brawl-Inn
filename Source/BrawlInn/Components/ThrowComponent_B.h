// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "ThrowComponent_B.generated.h"

class APlayerCharacter_B;
class AGameMode_B;
class UHoldComponent_B;

UCLASS()
class BRAWLINN_API UThrowComponent_B : public USphereComponent
{
	GENERATED_BODY()
public:
	UThrowComponent_B(const FObjectInitializer& ObjectInitializer);

	bool IsReady() const;
	bool TryThrow();
	void Throw();

	UFUNCTION(BlueprintPure)
	bool IsThrowing() const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void GameIsReady();

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImpulseSpeed = 1500.f;

private:

	bool bIsThrowing = false;

	TArray<APlayerCharacter_B*> OtherPlayers;

	AGameMode_B* GameMode = nullptr;
	UHoldComponent_B* HoldComponent = nullptr;
	APlayerCharacter_B* OwningPlayer = nullptr;
};
