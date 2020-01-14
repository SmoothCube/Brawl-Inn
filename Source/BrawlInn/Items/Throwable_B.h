// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Throwable_B.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EThrowableTypes : uint8
{
	ENone 	UMETA(DisplayName = "None"),
	EStool 	UMETA(DisplayName = "Stool"),
	EBarrel UMETA(DisplayName = "Barrel")
};

class USphereComponent;
class UNiagaraSystem;
class APlayerCharacter_B;

UCLASS()
class BRAWLINN_API AThrowable_B : public AActor
{
	GENERATED_BODY()
	
public:	
	AThrowable_B();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* PickupSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EThrowableTypes Type = EThrowableTypes::ENone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* ParticleSystem;

	void PickedUp(APlayerCharacter_B* Owner);

	void Dropped();

	bool IsHeld() const;

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	void OnThrowOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Variables")
	float ThrowHitStrength = 100;
private:

	APlayerCharacter_B* OwningPlayer = nullptr;
};
