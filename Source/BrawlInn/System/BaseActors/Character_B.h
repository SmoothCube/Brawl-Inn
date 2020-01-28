// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "System/Interfaces/ThrowableInterface_B.h"
#include "Character_B.generated.h"

class UHoldComponent_B;
class UThrowComponent_B;
class UPunchComponent_B;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector InputVector = FVector::ZeroVector;

	void SetState(EState s);
	EState GetState() const;

protected:

	// ** Overriden functions **
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	// ** Overlap/Hit functions **
	UFUNCTION()
		virtual void OnRadialDamageTaken(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser);



	EState State = EState::EWalking;

	FVector FindMeshLocation();


	FTransform RelativeMeshTransform;

	float NormalMaxWalkSpeed = 0;

	ACharacter_B* HoldingPlayer = nullptr;

	UPROPERTY(EditAnywhere, Category = "Variables", meta = (Tooltip = "The longest amount of time this character can be held"))
		float MaxHoldTime = 4.f;

	float CurrentHoldTime = 0.f;

	void BreakFree();

	void StandUp();


	UPROPERTY(EditAnywhere, Category = "Variables|Stun")
		float StunTime = 3.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Stun")
		int PunchesToStun = 2;

	int StunAmount = 0;

	void AddStun(int Strength = 1);
	void RemoveStun();

	FTimerHandle TH_StunTimer;
	FTimerHandle TH_RecoverTimer;

	UPROPERTY(EditAnywhere, Category = "Variables|Punch", meta = (Tooltip = "For when an external force made the character fall. Name is a bit misleading"))
		float PunchedRecoveryTime = 4.0;

	UFUNCTION()
		void Fall(float RecoveryTime);

	virtual bool CheckFall(float DeltaTime);

	void HandleMovement(float DeltaTime);
	void HandleRotation(float DeltaTime);



	UPROPERTY(EditAnywhere, Category = "Variables|Punch")
		float FallLimitMultiplier = 3.5f;

};
