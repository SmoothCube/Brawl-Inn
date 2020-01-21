// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter_B.generated.h"

class UHealthComponent_B;
class UHoldComponent_B;
class UThrowComponent_B;
class UFireDamageComponent_B;
class UPunchComponent_B;
class APlayerController_B;

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EState : uint8
{
	EWalking 	UMETA(DisplayName = "Walking"),
	EHolding 	UMETA(DisplayName = "Holding"),
	EFallen		UMETA(DisplayName = "Fallen")
};

UCLASS()
class BRAWLINN_API APlayerCharacter_B : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter_B();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPunchComponent_B* PunchComponent;

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
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
	
	// ** Overlap/Hit functions **
	UFUNCTION()
	void CapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRadialDamageTaken(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser);

	// ** Functions **
	void HandleMovement(float DeltaTime);
	void HandleMovementHold();

	void HandleRotation();

	UFUNCTION()
	void TakeFireDamage();

	UFUNCTION()
	void Fall();

	void StandUp();

	void MakeInvulnerable(float InvincibilityTime);
	void MakeVulnerable();

public:	

	UFUNCTION(BlueprintPure)
	APlayerController_B* GetPlayerController_B() const;
	
	void PunchButtonPressed();

	bool IsInvulnerable();

	// ** Variables **

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector InputVector = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RotationVector = FVector::ZeroVector;
	EState State = EState::EWalking;

protected: 

	UPROPERTY(EditAnywhere, Category = "Variables")
	float RecoveryTime = 2.0;

	UPROPERTY(EditAnywhere, Category = "Variables")
	float TimeBeforeFall = 1.f;

	UPROPERTY(EditAnywhere, Category = "Variables")
	float FallLimitMultiplier = 3.5f;

	UPROPERTY(EditAnywhere, Category = "Variables")
	int FellOutOfWorldDamageAmount = 1;

	UPROPERTY(EditAnywhere, Category = "Variables")
	FName ForceSocketName = "ProtoPlayer_BIND_SpineTop_JNT_center";

	UPROPERTY(EditAnywhere, Category = "Visuals")
	UMaterial* InvulnerableMat;

	UPROPERTY(EditAnywhere, Category = "Visuals")
	UMaterial* InvisibleMat;

	UPROPERTY(BlueprintReadOnly)
	bool bIsInvulnerable = false;


private:
	
	FTransform RelativeMeshTransform;
	FTimerHandle TH_RecoverTimer;
	FTimerHandle TH_InvincibilityTimer;
	
	float CurrentFallTime = 0.f;

	
	APlayerController_B* PlayerController = nullptr;
	friend class UPunchComponent_B;
	FVector FindMeshLocation();
};

