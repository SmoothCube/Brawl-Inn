// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SelectionPawn_B.generated.h"

class UPaperSpriteComponent;

UCLASS()
class BRAWLINN_API ASelectionPawn_B : public APawn
{
	GENERATED_BODY()
public:
	ASelectionPawn_B();

	// ********** AActor **********
protected:
	virtual void BeginPlay() override;

	// ********** Misc. **********
	
	UPROPERTY(VisibleAnywhere)
		UPaperSpriteComponent* SpriteIcon;
};
