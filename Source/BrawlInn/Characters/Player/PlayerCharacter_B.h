// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Character_B.h"
#include "Characters/Player/PlayerInfo.h"
#include "PlayerCharacter_B.generated.h"

class AController;
class UGameInstance_B;
class UDataTable_B;
class USoundCue;
class AGamePlayerController_B;

UCLASS()
class BRAWLINN_API APlayerCharacter_B : public ACharacter_B
{
	GENERATED_BODY()

public:
	APlayerCharacter_B();

	
	// ********** AActor **********
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// ********** Components **********

public:
	UStaticMeshComponent* GetDirectionIndicatorPlane() const;
protected:
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* DirectionIndicatorPlane = nullptr;

	// ********** Falling **********

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	UFUNCTION()
	void Die();

	virtual void Fall(FVector MeshForce = FVector::ZeroVector, float RecoveryTime = -1) override;
	virtual void StandUp() override;

	// ********** Hold players **********

	virtual void Dropped_Implementation() override;

public:
	void BreakFreeButtonMash();

protected:
	void BreakFree();

	UPROPERTY(EditAnywhere, Category = "Variables", meta = (Tooltip = "The longest amount of time this character can be held"))
		float MaxHoldTime = 3.f;

	float CurrentHoldTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Variables", meta = (Tooltip = "The time in seconds the press of a button decreases hold time"))
	float HoldTimeDecreasePerButtonMash = 0.05f;

	// ********** Damage **********

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void SetLastHitBy(AController* EventInstigator);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|UI")
		UTexture2D* ColoredHealthIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|UI")
		UTexture2D* GreyHealthIcon = nullptr;

	UPROPERTY(EditAnywhere, Category = "Variables|Info")
		float RespawnDelay = 1.f;

	FTimerHandle LastHitByTimer_TH;

	// ********** Shatter **********
	UPROPERTY(EditAnywhere, Category = "Variables|Audio")
		USoundCue* LowShatterSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Variables|Audio")
		USoundCue* MidShatterSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Variables|Audio")
		USoundCue* HighShatterSound = nullptr;
	
	
	// ********** Misc. **********

public:
	virtual void SetChargeLevel(EChargeLevel chargeLevel) override;
	virtual void AddStun(const int Strength = 1) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		FPlayerInfo PlayerInfo;

protected:
	UPROPERTY()
		AGamePlayerController_B* PlayerController = nullptr;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:

	UPROPERTY()
	UDataTable_B* Table = nullptr;

	UPROPERTY()
		UGameInstance_B* GameInstance = nullptr;
};
