// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragArea_B.generated.h"

class UBoxComponent;
class UStaticMeshComponent; //TODO delete?
class USkeletalMeshComponent;
class ACharacter_B;
class USoundCue;

UCLASS()
class BRAWLINN_API ADragArea_B : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADragArea_B();

	UPROPERTY(EditAnywhere, Category="Variables")
	float ItemDragStrength = 100000.0f;

	UPROPERTY(EditAnywhere, Category = "Variables")
	float InputMultiplier = 0.25;

	UPROPERTY(EditAnywhere, Category = "Variables")
	float SkeletalDragStrength= 4000.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* DragArea = nullptr;
	

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
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);
};
