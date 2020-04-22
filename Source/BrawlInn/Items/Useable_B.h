// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item_B.h"
#include "Useable_B.generated.h"

class UDestructibleComponent;
class UDestructibleMesh;
class USoundCue;
class UNiagaraComponent;

UCLASS()
class BRAWLINN_API AUseable_B : public AItem_B
{
	GENERATED_BODY()

public:

	AUseable_B();
	// ********** Components **********

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* DrinkMesh;

	UPROPERTY(VisibleAnywhere)
		UNiagaraComponent* NiagaraSystemComponent;

public:

	UDestructibleComponent* GetDestructibleComponent() const;
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UDestructibleComponent* DestructibleComponent;
	
	// ** Overridden functions **

	virtual void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PickedUp_Implementation(ACharacter_B* Player) override;

	virtual void Dropped_Implementation() override;

	virtual void Use_Implementation() override;
public:
	float GetUseTime();
protected:
	virtual void FellOutOfWorld(const UDamageType& dmgType) override;

	UPROPERTY(EditDefaultsOnly, Category = "Juice")
		USoundCue* DrinkSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Boost", meta = (ToolTip = "How long it takes to use the powerup"))
		float UseTime = 2.f;
  
	UPROPERTY(EditDefaultsOnly, Category = "Boost", meta = (ToolTip = "The duration of the powerup. Set to -1 if it shouldn't expire by time"))
		float Duration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boost", meta = (ToolTip = "The amount of time the camera will focus on this after it spawns"))
		float CameraFocusDuration = 3.f;
	
	UPROPERTY(EditDefaultsOnly)
		float ThrowAwayStrength = 100000;

	FTimerHandle TH_DrinkHandle;

	UFUNCTION()
		virtual void ResetBoost();
public:
	void ThrowAway(FVector Direction);

private:
	// ********** Destroy/Fracture **********

	FTimerHandle TH_Despawn;
	FTimerHandle TH_Destroy;
	
	void BeginDespawn();

	void StartDestroy();

	UPROPERTY(EditAnywhere, Category = "Despawning")
		float DownValuePerTick = 10;

	UPROPERTY(EditAnywhere, Category = "Despawning")
		float TimeBeforeDespawn = 5.f;

};
