// Fill out your copyright notice in the Description page of Project Settings.

#include "ThrowComponent_B.h"
#include "BrawlInn.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "System/Interfaces/ThrowableInterface_B.h"
#include "Player/PlayerCharacter_B.h"
#include "Components/HoldComponent_B.h"
#include "Items/Throwable_B.h"
#include "System/GameMode_B.h"

UThrowComponent_B::UThrowComponent_B(const FObjectInitializer& ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UThrowComponent_B::TryThrow()
{
	if (!IsReady())
	{
		BWarn("Not Ready To Throw");
		return false;
	}
	else if (!HoldComponent->IsHolding())
	{
		BWarn("Not Holding Item!");
	}
	else if (!(OwningPlayer->GetState() == EState::EHolding))
	{
		BWarn("Wrong Player State");
		return false;
	}

	BWarn("Trying Throw!");
	bIsThrowing = true;
	return true;
}

bool UThrowComponent_B::IsThrowing() const
{
	return bIsThrowing;
}

void UThrowComponent_B::BeginPlay()
{
	Super::BeginPlay();

	OwningPlayer = Cast<APlayerCharacter_B>(GetOwner());

	GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->SpawnCharacter_D.AddDynamic(this, &UThrowComponent_B::OneCharacterChanged);
		GameMode->DespawnCharacter_D.AddDynamic(this, &UThrowComponent_B::OneCharacterChanged);
	}
}

bool UThrowComponent_B::AimAssist(FVector& TargetPlayerLocation)
{
	if (OtherPlayers.Num() == 0)
		return false;

	FVector PlayerLocation = OwningPlayer->GetActorLocation();
	PlayerLocation.Z = 0;
	FVector PlayerForward = PlayerLocation + OwningPlayer->GetActorForwardVector() * AimAssistRange;
	PlayerForward.Z = 0;

	FVector PlayerToForward = PlayerForward - PlayerLocation;

	TArray<APlayerCharacter_B*> PlayersInCone;

	for (const auto& OtherPlayer : OtherPlayers)
	{
		if (OtherPlayer->GetState() == EState::EFallen)
			continue;
		FVector OtherPlayerLocation = OtherPlayer->GetActorLocation();
		OtherPlayerLocation.Z = 0;

		FVector PlayerToOtherPlayer = OtherPlayerLocation - PlayerLocation;
		if (PlayerToOtherPlayer.Size() > AimAssistRange)
			continue;

		float AngleA = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(PlayerToForward.GetSafeNormal(), PlayerToOtherPlayer.GetSafeNormal())));
		if (AngleA <= AimAssistAngle)
			PlayersInCone.Add(OtherPlayer);
	}
	APlayerCharacter_B* TargetPlayer = nullptr;

	switch (PlayersInCone.Num())
	{
	case 0:
		return false;
	case 1:
		TargetPlayer = PlayersInCone[0];
		break;
	default:
		PlayersInCone.Sort([&](const APlayerCharacter_B& LeftSide, const APlayerCharacter_B& RightSide)
			{
				FVector A = LeftSide.GetActorLocation();
				A.Z = 0;
				FVector B = RightSide.GetActorLocation();
				B.Z = 0;
				float DistanceA = FVector::Dist(PlayerLocation, LeftSide.GetActorLocation());
				float DistanceB = FVector::Dist(PlayerLocation, RightSide.GetActorLocation());
				return DistanceA < DistanceB;
			});
		TargetPlayer = PlayersInCone[0];
		break;
	}

	FVector TargetLocation = TargetPlayer->GetActorLocation();
	TargetLocation.Z = 0;
	FVector ThrowDirection = TargetLocation - PlayerLocation;
	TargetPlayerLocation = ThrowDirection.GetSafeNormal();
	return true;
}
void UThrowComponent_B::OneCharacterChanged(APlayerController_B* PlayerController)
{
	/// Finds all characters
	OtherPlayers.Empty();
	TArray<AActor*> TempArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter_B::StaticClass(), TempArray);
	for (const auto& Player : TempArray)
	{
		if (Cast<APlayerCharacter_B>(Player) == OwningPlayer)
			continue;
		//	BScreen("Player %s, %i", *GetNameSafe(Player), TempArray.Num());
		OtherPlayers.Add(Cast<APlayerCharacter_B>(Player));
	}

	/// Finds the holdcomponent.
	// Dette kan kanskje flyttes til BeginPlay ? Usikker på hvilke av komponentene som blir laget først
	HoldComponent = OwningPlayer->HoldComponent;
}

void UThrowComponent_B::Throw()
{
	BWarn("Throw start!");
	if (!OwningPlayer)
	{
		BError("No OwningPlayer for ThrowComponent %s!", *GetNameSafe(this));
		return;
	}
	if (!HoldComponent)
	{
		BError("No HoldComponent for ThrowComponent");
	}
	if (!HoldComponent->IsHolding())
	{
		OwningPlayer->SetState(EState::EWalking);
		return;
	}
	BWarn("Throw mid!");

	/// Prepare item to be thrown
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(HoldComponent->GetHoldingItem());
	if (Interface)
	{
		Interface->Execute_Use(HoldComponent->GetHoldingItem());
	}

	/// Throw with the help of AimAssist.
	//FVector TargetLocation = OwningPlayer->GetActorForwardVector();   //Had a crash here, called from notify PlayerThrow_B. Added pointer check at top of function
	//AimAssist(TargetLocation);
	//if (HoldComponent->IsHolding())
	//{
	//	HoldComponent->GetHoldingItem()->Mesh->AddImpulse(TargetLocation * ImpulseSpeed, NAME_None, true);
	//}
	OwningPlayer->HoldComponent->SetHoldingItem(nullptr);
	bIsThrowing = false;
	OwningPlayer->SetState(EState::EWalking);
	BWarn("Throw end!");

}

bool UThrowComponent_B::IsReady() const
{
	if (!GameMode)
	{
		BWarn("No game mode!");
		return false;
	}
	if (!OwningPlayer)
	{
		BWarn("No Owning Player");
		return false;
	}
	else if (!HoldComponent)
	{
		BWarn("No Hold Component!");
		return false;
	}
	return true;

}
