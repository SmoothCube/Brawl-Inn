// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Character_B.h"
#include "Characters/Player/PlayerInfo.h"
#include "PlayerCharacter_B.generated.h"

class AController;

UCLASS()
class BRAWLINN_API APlayerCharacter_B : public ACharacter_B
{
	GENERATED_BODY()

public:

	APlayerCharacter_B();
	
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void Die();

	void StartRespawn();
	
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* DirectionIndicatorPlane = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|UI")
		UTexture2D* ColoredHealthIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals|UI")
		UTexture2D* GreyHealthIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	EPlayerCharacterType Type;

protected:
	UPROPERTY(EditAnywhere, Category = "Variables|Respawn")
	float RespawnDelay = 3.f;

	FTimerHandle TH_RespawnTimer;

};
