// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Throwable_B.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EThrowableTypes : uint8
{
	ENone 	UMETA(DisplayName = "None"),
	EStool 	UMETA(DisplayName = "Stool")
};

class USphereComponent;

UCLASS()
class BRAWLINN_API AThrowable_B : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThrowable_B();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* PickupSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EThrowableTypes Type = EThrowableTypes::ENone;

	void PickedUp();

	void Dropped();

	bool IsHeld() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
private:

	bool bIsHeld = false;
};
