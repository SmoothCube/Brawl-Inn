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

	UPROPERTY(EditAnywhere, Category = "Movement")
		float NormalRotationInterpSpeed = 10.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
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

	UPROPERTY(EditAnywhere, Category = "Fall", meta = (Tooltip = "For when an external force made the character fall."))
		float FallRecoveryTime = 5.f;

	UPROPERTY(EditAnywhere, Category = "Fall")
		float FallLimitMultiplier = 1.5;

	FTimerHandle TH_FallRecoverTimer;

	// ********** Hold players **********
public:
	virtual void PickedUp_Implementation(ACharacter_B* Player) override;

	virtual void Dropped_Implementation() override;

	virtual void Use_Implementation() override;

	virtual bool IsHeld_Implementation() const override;

	virtual bool CanBeHeld_Implementation() const override;

	virtual float GetThrowStrength_Implementation() const override;

	float GetMovementSpeedWhenHeld_Implementation() const override;

	virtual float GetPickupWeight_Implementation() const override;

	
	UFUNCTION()
		virtual void OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void OnCapsuleOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	EState State = EState::EWalking;

	UPROPERTY()
		ACharacter_B* HoldingCharacter = nullptr;

	UPROPERTY(EditAnywhere, Category = "Being Held")
		float TimeBeforeThrowCollision = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Throw", meta = (Tooltip = "Used to prioritize what item gets picked up when more than one is avaliable. Higher values will be chosen."))
		float PickupWeight = 2.f;

	bool bCanBeHeld = false;
	FTimerHandle TH_FallCollisionTimer;

	UPROPERTY(EditAnywhere, Category = "Being Held")
		float MovementSpeedWhenHeld = 600.f;

public:

	const FRotator GetHoldRotation_Implementation() override;

	const FVector GetHoldLocation_Implementation() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Being Held")
	FRotator HoldRotation = FRotator(0, 0, 0);
	
	UPROPERTY(EditAnywhere, Category = "Being Held")
	FVector HoldLocation = FVector(0, 0, 575);

public:

	// ********** Charge **********
	EChargeLevel GetChargeLevel();
	bool IsCharging() const;
	virtual void SetChargeLevel(EChargeLevel chargeLevel);

	void SetIsCharging(bool Value);

protected:
	UPROPERTY(EditAnywhere, Category = "Throw")
		float ThrowStrength = 200000.f;

	UPROPERTY(EditAnywhere, Category = "Charge")
		float Charge2MoveSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Charge")
		float Charge3MoveSpeed = 350.f;

	bool bIsCharging = false;

	EChargeLevel ChargeLevel = EChargeLevel::ENotCharging;

protected:

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* ChargeLevelSound = nullptr;
public:


	// ********** Stun **********
public:
	virtual void AddStun(const int Strength = 1);

	int GetPunchesToStun();

	int GetStunAmount();

	UPROPERTY(EditAnywhere, Category = "Stun")
		int StunStrength = 1;

protected:
	UPROPERTY(EditAnywhere, Category = "Stun")
		float StunTime = 3.f;

	UPROPERTY(EditAnywhere, Category = "Stun")
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
	UPROPERTY(EditAnywhere, Category = "Invulnerability")
		float InvulnerabilityTime = 3.f;

	UPROPERTY(BlueprintReadOnly)
		bool bIsInvulnerable = false;

	UPROPERTY(EditAnywhere, Category = "Visuals")
		UMaterialInstance* InvulnerableMat;

	UPROPERTY(EditAnywhere, Category = "Visuals")
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
	UPROPERTY(EditAnywhere, Category = "Punch")
		FName ForceSocketName = "spine5_export_C_jnt";

	UPROPERTY(VisibleAnywhere, Category = "Punch")
		UNiagaraComponent* PS_Charge = nullptr;

	// ********** Damage **********

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere, Category = "Damage")
		int FellOutOfWorldScoreAmount = 100;

	UPROPERTY(EditAnywhere, Category = "Damage")
		int SuicideScoreAmount = -50;

	UPROPERTY(EditAnywhere, Category = "Damage")
		int FallScoreAmount = 25;

	UPROPERTY(EditAnywhere, Category = "Damage")
		int StoolScoreAmount = 25;

	UPROPERTY(EditAnywhere, Category = "Damage")
		int PowerupKnockbackScoreAmount = 25;

	UPROPERTY(EditAnywhere, Category = "Audio")
		USoundCue* HurtSound;
public:
	// ********** Powerup **********
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Powerup", meta = (Tooltip = "The strength of which this character will be knocked back when hit by a powered up player"))
		float PowerupPushStrength = 80000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Powerup", meta = (Tooltip = "The upwards angle of the powerup knockback"))
		float PowerupUpwardsAngle = 15.f;
protected:
	// ********** Misc. **********

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals", meta = (Tooltip = "The material index that special effects like invulnerability will be applied to"))
		int SpecialMaterialIndex = 6;

	FTransform RelativeMeshTransform;

	friend class UPunchComponent_B;
	friend class UThrowComponent_B;

public:

	bool IsAlive() const;

protected:

	bool bIsAlive = true;

};
