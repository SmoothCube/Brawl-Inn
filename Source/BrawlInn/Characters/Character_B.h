// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "System/Interfaces/ThrowableInterface_B.h"
#include "Character_B.generated.h"

class AGameCamera_B;
class UHoldComponent_B;
class UThrowComponent_B;
class UPunchComponent_B;
class UMaterial;
class USoundCue;
class UNiagaraComponent;
class UMaterialInstance;

UENUM(BlueprintType)
enum class EState : uint8
{
	EWalking 	UMETA(DisplayName = "Walking"),
	EHolding 	UMETA(DisplayName = "Holding"),
	EFallen		UMETA(DisplayName = "Fallen"),
	EBeingHeld 	UMETA(DisplayName = "BeingHeld"),
	EPoweredUp 	UMETA(DisplayName = "PoweredUp")
};

UCLASS()
class BRAWLINN_API ACharacter_B : public ACharacter, public IThrowableInterface_B
{
	GENERATED_BODY()

public:
	ACharacter_B();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UPunchComponent_B* PunchComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UHoldComponent_B* HoldComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UThrowComponent_B* ThrowComponent;

protected:

	// ********** AActor **********

	virtual void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

	// ********** Movement **********
public:
	void SetInputVectorX(const float X);

	void SetInputVectorY(const float Y);

protected:

	void HandleMovement(float DeltaTime);
public:
	float NormalMaxWalkSpeed = 0;
	void SetCanMove(bool Value);

	bool GetCanMove();

protected:
	float RotationInterpSpeed = 10.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Movement")
		float NormalRotationInterpSpeed = 10.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Movement")
		bool bCanMove = true;

public:

	void SetGameCamera(AGameCamera_B* Camera);
protected:
	UPROPERTY()
		AGameCamera_B* GameCamera = nullptr;
	
	FVector InputVector = FVector::ZeroVector;

	// ********** Falling **********
public:
	void CheckFall(FVector MeshForce);
protected:
	virtual void Fall(FVector MeshForce = FVector::ZeroVector, float RecoveryTime = -1, bool bPlaySound = true);

	virtual void StandUp();

	FVector FindMeshLocation() const;

	FVector FindMeshGroundLocation() const;

	UPROPERTY(EditAnywhere, Category = "Variables|Fall", meta = (Tooltip = "For when an external force made the character fall."))
		float FallRecoveryTime = 2.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Fall")
		float FallLimitMultiplier = 1.5;

	FTimerHandle TH_FallRecoverTimer;

	// ********** Hold players **********
public:
	virtual void PickedUp_Implementation(ACharacter_B* Player) override;

	virtual void Dropped_Implementation() override;

	virtual void Use_Implementation() override;

	virtual bool IsHeld_Implementation() const override;

	virtual bool CanBeHeld_Implementation() const override;

	virtual float GetThrowStrength_Implementation(EChargeLevel level) const override;

	float GetMovementSpeedWhenHeld_Implementation() const override;
	
	UFUNCTION()
		virtual void OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	EState State = EState::EWalking;

	UPROPERTY()
		ACharacter_B* HoldingCharacter = nullptr;

	UPROPERTY(EditAnywhere, Category = "Variables|Being Held")
		float TimeBeforeThrowCollision = 0.3f;

	bool bCanBeHeld = true;
	FTimerHandle TH_FallCollisionTimer;

	UPROPERTY(EditAnywhere, Category = "Variables|Being Held")
		float MovementSpeedWhenHeld = 600.f;

public:

	FRotator& GetHoldRotation();

protected:
	UPROPERTY(EditAnywhere, Category = "Variables|Being Held")

		FRotator HoldRotation = FRotator(0, 0, 0);

	FVector HoldOffset = FVector(0, 0, 0);

public:
	// ********** Holding Drink **********
	
	FVector HoldingDrinkOffset = FVector::ZeroVector;

	// ********** Charge **********
	EChargeLevel GetChargeLevel();
	bool IsCharging() const;
	virtual void SetChargeLevel(EChargeLevel chargeLevel);

	void SetIsCharging(bool Value);

protected:
	UPROPERTY(EditAnywhere, Category = "Variables|Throw")
		float Charge1ThrowStrength = 200000.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Throw")
		float Charge2ThrowStrength = 250000.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Throw")
		float Charge3ThrowStrength = 500000.f;


	UPROPERTY(EditAnywhere, Category = "Variables|Charge")
		float Charge2MoveSpeed = 500.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Charge")
		float Charge3MoveSpeed = 100.f;

	bool bIsCharging = false;

	EChargeLevel ChargeLevel = EChargeLevel::ENotCharging;
public:

	UPROPERTY(EditAnywhere, Category = "Variables|Charge")
		float ChargeTier2Percentage = 0.2;

	UPROPERTY(EditAnywhere, Category = "Variables|Charge")
		float ChargeTier3Percentage = 0.9;
protected:

	UPROPERTY(EditAnywhere, Category = "Variables|Audio")
		USoundCue* ChargeLevelSound = nullptr;
public:


	// ********** Stun **********
public:
	virtual void AddStun(const int Strength = 1);

	UPROPERTY(EditAnywhere, Category = "Variables|Stun")
		int StunStrength = 1;

protected:
	UPROPERTY(EditAnywhere, Category = "Variables|Stun")
		float StunTime = 3.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Stun")
		int PunchesToStun = 1;

	UPROPERTY(VisibleAnywhere)
		UNiagaraComponent* PS_Stun = nullptr;

	int StunAmount = 0;

	FTimerHandle TH_StunTimer;


	// ********** Special Material **********
public:
	void SetSpecialMaterial(UMaterialInstance* Material);
	void RemoveSpecialMaterial();

	// ********** Invulnerability **********

	/// A number less than 0 will make the character invulnerable until toggled off again
	void MakeInvulnerable(float ITime = -1, bool bShowInvulnerabilityEffect = true);

	bool IsInvulnerable();

	void MakeVulnerable();

protected:
	UPROPERTY(EditAnywhere, Category = "Variables|Invulnerability")
		float InvulnerabilityTime = 3.f;

	UPROPERTY(BlueprintReadOnly)
		bool bIsInvulnerable = false;

	UPROPERTY(EditAnywhere, Category = "Variables|Visuals")
		UMaterialInstance* InvulnerableMat;

	UPROPERTY(EditAnywhere, Category = "Variables|Visuals")
		UMaterial* InvisibleMat;

	FTimerHandle TH_InvincibilityTimer;

	// ********** Shield **********
public:
	void ApplyShield();

	void RemoveShield();

	bool HasShield() const;

protected:
	bool bHasShield = false;

	UPROPERTY(EditAnywhere, Category = "Visuals")
		UMaterialInstance* ShieldMat;

	// ********** States **********

public:
	virtual void SetState(EState s);

	UFUNCTION(BlueprintCallable)
		EState GetState() const;

	UFUNCTION()
		virtual void Die();
	// ********** Punch **********
public:
	void TryStartCharging();

	UNiagaraComponent* GetChargeParticle() const;

protected:
	UPROPERTY(EditAnywhere, Category = "Variables|Punch")
		FName ForceSocketName = "ProtoPlayer_BIND_SpineTop_JNT_center";

	UPROPERTY(VisibleAnywhere, Category = "Variables|Punch")
		UNiagaraComponent* PS_Charge = nullptr;

	// ********** Damage **********

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, Category = "Variables|Damage")
		int FellOutOfWorldScoreAmount = 100;

	UPROPERTY(EditAnywhere, Category = "Variables|Damage")
		int FallScoreAmount = 25;

	UPROPERTY(EditAnywhere, Category = "Variables|Damage")
		int StoolScoreAmount = 25;

	UPROPERTY(EditAnywhere, Category = "Variables|Damage")
		int PowerupKnockdownScoreAmount = 25;

	UPROPERTY(EditAnywhere, Category = "Variables|Audio")
		USoundCue* HurtSound;

	// ********** Misc. **********

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables|Visuals", meta = (Tooltip = "The material index that special effects like invulnerability will be applied to"))
		int SpecialMaterialIndex = 6;

	FTransform RelativeMeshTransform;

	friend class UPunchComponent_B;
	friend class UThrowComponent_B;

public:

	bool IsAlive() const;

protected:

	bool bIsAlive = true;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (Tooltip = "The strength og which this character will be knocked back when hit by a powered up player"))
	float PowerupPushStrength = 8000.f;
};
