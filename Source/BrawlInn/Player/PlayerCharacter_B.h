// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/BaseActors/Character_B.h"
#include "PlayerCharacter_B.generated.h"

class UStaticMeshComponent;
class UHealthComponent_B;
class AGameCamera_B;
class APlayerController_B;
class UWidgetComponent;

UCLASS()
class BRAWLINN_API APlayerCharacter_B : public ACharacter_B
{
	GENERATED_BODY()

public:
	APlayerCharacter_B();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UWidgetComponent* HealthWidget;

protected:

	// ** Overriden functions **
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void UpdateHealthRotation(); //Move into health widget
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
	
	// ** Overlap/Hit functions **
	virtual void OnRadialDamageTaken(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo, class AController* InstigatedBy, AActor* DamageCauser) override;

	// ** Functions **

	void MakeVulnerable();

public:	

	UFUNCTION(BlueprintPure)
	APlayerController_B* GetPlayerController_B() const;
	
	void PunchButtonPressed();

	void BreakFreeButtonMash();

	void MakeInvulnerable(float InvulnerabilityTime);
	bool IsInvulnerable();

	void ApplyShield();
	void RemoveShield();

	//Picking up players
	virtual void PickedUp_Implementation(APlayerCharacter_B* Player) override;

	virtual void Dropped_Implementation() override;

	virtual bool IsHeld_Implementation() const override;

	virtual void Use_Implementation() override;

	// ** Variables **

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RotationVector = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category = "Variables|Invulnerability")
	float InvulnerabilityTime = 3.f;

	UPROPERTY(EditAnywhere, Category = "Variables|Stun")
		int StunStrength = 1;
	
	bool bHasShield = false;
protected: 


	UPROPERTY(EditAnywhere, Category = "Variables|Punch", meta = (Tooltip = "For when a character fell by themselves"))
	float FellRecoveryTime = 2.0;				

	virtual bool CheckFall(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category = "Variables|Damage")
	int FellOutOfWorldDamageAmount = 1;

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
private:
	

	FTimerHandle TH_InvincibilityTimer;
	
	APlayerController_B* PlayerController = nullptr;
	AGameCamera_B* GameCamera = nullptr;

	friend class UPunchComponent_B;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DirectionIndicatorPlane = nullptr;
};
