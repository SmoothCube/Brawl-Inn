// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Character_B.h"
#include "Characters/Player/PlayerInfo.h"
#include "System/Structs/ScoreValues.h"
#include "PlayerCharacter_B.generated.h"

class UScoreText_B;
class UUserWidget;
class UWidgetComponent;
class UNiagaraSystem;
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

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	// ********** Components **********

public:
	UStaticMeshComponent* GetDirectionIndicatorPlane() const;

	UNiagaraComponent* GetChiliBrewParticle() const;

	USoundCue* GetChiliBrewSound() const;

protected:
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* DirectionIndicatorPlane = nullptr;

	UPROPERTY(VisibleAnywhere)
		UNiagaraComponent* PS_ChiliBrew = nullptr;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* ChiliBrewSound = nullptr;

	UPROPERTY(VisibleAnywhere)
		UPawnNoiseEmitterComponent* NoiseEmitterComponent = nullptr;

	// ********** Movement **********

	void HandleMovementPoweredUp(float DeltaTime);

	FVector PrevInputVector;

	int DashThroughScoreValue = 5.f;
	// ********** Falling **********

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
public:
	virtual void Die() override;
protected:
	virtual void Fall(FVector MeshForce = FVector::ZeroVector, float RecoveryTime = -1, bool bPlaySound = true) override;

	virtual void StandUp() override;

	// ********** Hold players **********
	virtual void PickedUp_Implementation(ACharacter_B* Player) override;

	virtual void Dropped_Implementation() override;

public:
	void BreakFreeButtonMash();

protected:
	void BreakFree();

	UPROPERTY(EditAnywhere, meta = (Tooltip = "The longest amount of time this character can be held"))
		float MaxHoldTime = 3.f;

	float CurrentHoldTime = 0.f;
public:
	float BreakFreeAnimationBlend = 0.f;
protected:
	UPROPERTY(EditAnywhere, meta = (Tooltip = "The time in seconds the press of a button decreases hold time"))
		float HoldTimeDecreasePerButtonMash = 0.05f;

	// ********** Score **********

	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void SetLastHitBy(AController* EventInstigator);

	void DisplayScoreVisuals(FScoreValues ScoreValues);

	FDelegateHandle DisplayScoreVisualsHandle;
	
	UFUNCTION(BlueprintImplementableEvent)
		void ScoreParticleTimerStart();

	UFUNCTION(BlueprintCallable)
		void OnScoreParticleTimerUpdate(float Value);

	UFUNCTION(BlueprintCallable)
		void OnScoreParticleTimerFinished();

	UPROPERTY(EditDefaultsOnly, Category = "Score|Visuals")
		UNiagaraSystem* ScoreParticle = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Score|Audio")
		USoundCue* ScoreSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Score|Visuals")
		TSubclassOf<UScoreText_B> BP_ScoreTextWidget = nullptr;

	UScoreText_B* ScoreTextWidget = nullptr;

	UPROPERTY(VisibleAnywhere)
		UWidgetComponent* ScoreTextWidgetComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Score")
		float RespawnDelay = 1.f;

	FTimerHandle LastHitByTimer_TH;

	// ********** Shatter **********
	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* LowShatterSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* MidShatterSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* HighShatterSound = nullptr;


	// ********** Misc. **********

public:
	virtual void SetChargeLevel(EChargeLevel chargeLevel) override;
	virtual void AddStun(const int Strength = 1) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FPlayerInfo PlayerInfo;

protected:
	UPROPERTY()
		AGamePlayerController_B* PlayerController = nullptr;

	virtual void PossessedBy(AController* NewController) override;

	void UnPossessed() override;

	virtual void OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	FTimerHandle TH_MakeNoiseTimer;

	UPROPERTY()
		UGameInstance_B* GameInstance = nullptr;
};
