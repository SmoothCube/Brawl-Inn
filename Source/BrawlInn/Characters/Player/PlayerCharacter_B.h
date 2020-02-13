// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Character_B.h"
#include "Characters/Player/PlayerInfo.h"
#include "PlayerCharacter_B.generated.h"

class AController;
class APlayerController_B;

UCLASS()
class BRAWLINN_API APlayerCharacter_B : public ACharacter_B
{
	GENERATED_BODY()

public:

	APlayerCharacter_B();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void Dropped_Implementation() override;

	virtual void RemoveStun() override;


	
	UFUNCTION()
	void Die();

	virtual void Fall(float RecoveryTime = -1) override;

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* DirectionIndicatorPlane = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|UI")
		UTexture2D* ColoredHealthIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|UI")
		UTexture2D* GreyHealthIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		FPlayerInfo PlayerInfo;

	APlayerController_B* PlayerController = nullptr;


	void BreakFreeButtonMash();

protected:
	void BreakFree();

	UPROPERTY(EditAnywhere, Category = "Variables", meta = (Tooltip = "The longest amount of time this character can be held"))
		float MaxHoldTime = 4.f;

private:
	float CurrentHoldTime = 0.f;

protected:
	UPROPERTY(EditAnywhere, Category = "Variables|Info")
	float RespawnDelay = 3.f;


};
