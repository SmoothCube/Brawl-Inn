// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "System/Interfaces/ThrowableInterface_B.h"
#include "Character_B.generated.h"

class UStaticMeshComponent;
class UHealthComponent_B;
class UHoldComponent_B;
class UThrowComponent_B;
class UPunchComponent_B;
class AGameCamera_B;
class APlayerController_B;
class UWidgetComponent;
class UMaterial;
class USoundCue;
class UNiagaraComponent;

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EState : uint8
{
	EWalking 	UMETA(DisplayName = "Walking"),
	EHolding 	UMETA(DisplayName = "Holding"),
	EFallen		UMETA(DisplayName = "Fallen"),
	EStunned	UMETA(DisplayName = "Stunned"),
	EBeingHeld 	UMETA(DisplayName = "BeingHeld")
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

	// ** Overriden functions **
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	// ** Overlap/Hit functions **
	UFUNCTION()
		void OnRadialDamageTaken(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser);

	// ** Functions **
	void HandleMovement(float DeltaTime);
	void CheckFall(float DeltaTime);

	void HandleRotation(float DeltaTime);

	UFUNCTION()
		void Fall(float RecoveryTime);

	void StandUp();

	void MakeVulnerable();

	void RemoveStun();

	FVector FindMeshLocation();
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:

	UFUNCTION(BlueprintPure)
		APlayerController_B* GetPlayerController_B() const;

	void PunchButtonPressed();

	void BreakFreeButtonMash();
	void BreakFree();

	void MakeInvulnerable(float InvulnerabilityTime);
	bool IsInvulnerable();

	void ApplyShield();
	void RemoveShield();

	void AddStun(int Strength = 1);


	//Picking up players
	virtual void PickedUp_Implementation(ACharacter_B* Player) override;

	virtual void Dropped_Implementation() override;

	virtual bool IsHeld_Implementation() const override;

	virtual void Use_Implementation() override;

	void SetState(EState s);
	EState GetState() const;

	// ** Variables **

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector InputVector = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RotationVector = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Variables|Invulnerability")
		float InvulnerabilityTime = 3.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Stun")
		int StunStrength = 1;

	//The longest amount of time this character can be held
	UPROPERTY(EditAnywhere, Category = "Variables")
		float MaxHoldTime = 4.f;

	bool bHasShield = false;

	UPROPERTY(VisibleAnywhere)
		UNiagaraComponent* PS_Charge = nullptr;

protected:
	EState State = EState::EWalking;

	UPROPERTY(EditAnywhere, Category = "Variables|Punch", meta = (Tooltip = "For when a character fell by themselves"))
		float FellRecoveryTime = 2.0;

	UPROPERTY(EditAnywhere, Category = "Variables|Punch", meta = (Tooltip = "For when an external force made the character fall. Name is a bit misleading"))
		float PunchedRecoveryTime = 2.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Punch")
		float FallLimitMultiplier = 1.5;

	UPROPERTY(EditAnywhere, Category = "Variables|Damage")
		int FellOutOfWorldDamageAmount = 100;

	UPROPERTY(EditAnywhere, Category = "Variables|Damage")
		int FallDamageAmount = 25;

	UPROPERTY(EditAnywhere, Category = "Variables|Damage")
		int ChairDamageAmount = 25;

	UPROPERTY(EditAnywhere, Category = "Variables|Punch")
		FName ForceSocketName = "ProtoPlayer_BIND_SpineTop_JNT_center";

	UPROPERTY(EditAnywhere, Category = "Visuals")
		UMaterial* InvulnerableMat;

	UPROPERTY(EditAnywhere, Category = "Visuals")
		UMaterial* InvisibleMat;

	UPROPERTY(EditAnywhere, Category = "Visuals")
		UMaterial* ShieldMat;

	UPROPERTY(BlueprintReadOnly)
		bool bIsInvulnerable = false;

	UPROPERTY(EditAnywhere, Category = "Variables|Stun")
		float StunTime = 3.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Stun")
		int PunchesToStun = 2;

	int StunAmount = 0;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* HurtSound;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* PS_Stun = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UDamageType> BP_FallDamageType;

	APlayerController_B* PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (Tooltip = "The material index that special effects like invulnerability will be applied to"))
	int SpecialMaterialIndex = 6;

private:

	ACharacter_B* HoldingPlayer = nullptr;

	FTransform RelativeMeshTransform;
	FTimerHandle TH_RecoverTimer;
	FTimerHandle TH_InvincibilityTimer;
	FTimerHandle TH_StunTimer;

	float NormalMaxWalkSpeed;

	AGameCamera_B* GameCamera = nullptr;

	friend class UPunchComponent_B;

	float CurrentHoldTime = 0.f;

};
