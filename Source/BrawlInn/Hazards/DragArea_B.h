// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragArea_B.generated.h"

class UAudioComponent;
class UBoxComponent;
class UStaticMeshComponent; //TODO delete?
class USkeletalMeshComponent;
class ACharacter_B;
class USoundCue;

UCLASS()
class BRAWLINN_API ADragArea_B : public AActor
{
	GENERATED_BODY()

	ADragArea_B();

protected:
	// ********** Components **********
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UAudioComponent* RiverSoundComponent;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* DragArea = nullptr;

public:
	// ********** Overridden functions **********
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

public:	
	// Sets default values for this actor's properties

	// ********** Drag **********
	UPROPERTY(EditAnywhere)
	float ItemDragStrength = 100000.0f;

	UPROPERTY(EditAnywhere)
	float InputMultiplier = 0.25;

	UPROPERTY(EditAnywhere)
	float SkeletalDragStrength= 4000.f;
protected:
	//for characters that has not fallen (movement components)
	UPROPERTY()
	TArray<ACharacter_B*> PlayersToMove;
	//for all skeletal meshes? 
	UPROPERTY()
	TArray<USkeletalMeshComponent*> SkeletonsToMove;
	//for all other objects
	UPROPERTY()
	TArray<UPrimitiveComponent*> ComponentsToMove;

public:	
	// ********** Overlaps **********
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);
};
