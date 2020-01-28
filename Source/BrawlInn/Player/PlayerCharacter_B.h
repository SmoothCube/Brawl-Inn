// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "System/Interfaces/ThrowableInterface_B.h"
#include "PlayerCharacter_B.generated.h"

class UStaticMeshComponent;
class UHealthComponent_B;
class UHoldComponent_B;
class UThrowComponent_B;
class UFireDamageComponent_B;
class UPunchComponent_B;
class AGameCamera_B;
class APlayerController_B;
class UWidgetComponent;

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EState : uint8
{
	EWalking 	UMETA(DisplayName = "Walking"),
	EHolding 	UMETA(DisplayName = "Holding"),
	EFallen		UMETA(DisplayName = "Fallen"),
	EStunned	UMETA(DisplayName = "Stunned"),
	EBeingHeld 	UMETA(DisplayName = "BeingHeld")
};

UCLASS(DontCollapseCategories)
class BRAWLINN_API APlayerCharacter_B : public ACharacter, public IThrowableInterface_B
{
	GENERATED_BODY()

public:
	APlayerCharacter_B();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPunchComponent_B* PunchComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UWidgetComponent* HealthWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHoldComponent_B* HoldComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UThrowComponent_B* ThrowComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UFireDamageComponent_B* FireDamageComponent;


protected:

	// ** Overriden functions **
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void UpdateHealthRotation();
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
	
	// ** Overlap/Hit functions **
	UFUNCTION()
	void OnRadialDamageTaken(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser);

	// ** Functions **
	void HandleMovement(float DeltaTime);
	void CheckFall(float DeltaTime);
	void HandleMovementHold();

	void HandleRotation(float DeltaTime);

	UFUNCTION()
	void TakeFireDamage();

	UFUNCTION()
	void Fall(float RecoveryTime);

	void StandUp();

	void MakeVulnerable();

	void RemoveStun();

	FVector FindMeshLocation();


public:	

	UFUNCTION(BlueprintPure)
	APlayerController_B* GetPlayerController_B() const;
	
	void PunchButtonPressed();

	void MakeInvulnerable(float InvulnerabilityTime);
	bool IsInvulnerable();

	void AddStun(int Strength = 1);


	//Picking up players
	virtual void PickedUp_Implementation(APlayerCharacter_B* Player) override;

	virtual void Dropped_Implementation() override;

	virtual bool IsHeld_Implementation() const override;

	virtual void Use_Implementation() override;

	// ** Variables **

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector InputVector = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RotationVector = FVector::ZeroVector;

	
	UPROPERTY(EditAnywhere, Category = "Variables|Invulnerability")
	float InvulnerabilityTime = 3.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Stun")
		int StunStrength = 1;

	void SetState(EState s);
	EState GetState() const;

protected: 

	EState State = EState::EWalking;

	UPROPERTY(EditAnywhere, Category = "Variables|Punch", meta = (Tooltip = "For when a character fell by themselves"))
	float FellRecoveryTime = 2.0;				

	UPROPERTY(EditAnywhere, Category = "Variables|Punch", meta = (Tooltip = "For when an external force made the character fall. Name is a bit misleading"))
	float PunchedRecoveryTime= 4.0;				

	UPROPERTY(EditAnywhere, Category = "Variables|Punch")
	float FallLimitMultiplier = 3.5f;

	UPROPERTY(EditAnywhere, Category = "Variables|Damage")
	int FellOutOfWorldDamageAmount = 1;

	UPROPERTY(EditAnywhere, Category = "Variables|Punch")
	FName ForceSocketName = "ProtoPlayer_BIND_SpineTop_JNT_center";

	UPROPERTY(EditAnywhere, Category = "Visuals")
	UMaterial* InvulnerableMat;

	UPROPERTY(EditAnywhere, Category = "Visuals")
	UMaterial* InvisibleMat;

	UPROPERTY(BlueprintReadOnly)
	bool bIsInvulnerable = false;

	UPROPERTY(EditAnywhere, Category = "Variables|Stun")
	float StunTime = 3.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Stun")
	int PunchesToStun = 2;

	int StunAmount = 0;

private:
	
	APlayerCharacter_B* HoldingPlayer = nullptr;

	FTransform RelativeMeshTransform;
	FTimerHandle TH_RecoverTimer;
	FTimerHandle TH_InvincibilityTimer;
	FTimerHandle TH_StunTimer;
	
	float NormalMaxWalkSpeed;
	
	APlayerController_B* PlayerController = nullptr;
	AGameCamera_B* GameCamera = nullptr;

	friend class UPunchComponent_B;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DirectionIndicatorPlane = nullptr;
};

