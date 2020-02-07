// Fill out your copyright notice in the Description page of Project Settings.

#include "ThrowComponent_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraComponent.h"

#include "BrawlInn.h"
#include "System/Interfaces/ThrowableInterface_B.h"
#include "Characters/Character_B.h"
#include "Components/HoldComponent_B.h"
#include "Items/Throwable_B.h"
#include "System/GameModes/GameMode_B.h"

UThrowComponent_B::UThrowComponent_B(const FObjectInitializer& ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
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

	//BWarn("Trying Charge!");
	bIsCharging = true;
	if (OwningPlayer && OwningPlayer->PS_Charge)
		OwningPlayer->PS_Charge->Activate();
	return true;
}

bool UThrowComponent_B::IsCharging() const
{
	return bIsCharging;
}

void UThrowComponent_B::BeginPlay()
{
	Super::BeginPlay();

	OwningPlayer = Cast<ACharacter_B>(GetOwner());

	GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->SpawnCharacter_NOPARAM_D.AddDynamic(this, &UThrowComponent_B::OneCharacterChanged);
	GameMode->DespawnCharacter_NOPARAM_D.AddDynamic(this, &UThrowComponent_B::OneCharacterChanged);
}

void UThrowComponent_B::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bIsCharging)
	{
		ImpulseSpeed = MinImpulseSpeed + ((MaxImpulseSpeed - MinImpulseSpeed) * ImpulseTimer);
	}
}


bool UThrowComponent_B::IsThrowing() const
{
	return bIsThrowing;
}

void UThrowComponent_B::StartThrow()
{
	if (bIsCharging)
	{
		bIsCharging = false;
		if(OwningPlayer && OwningPlayer->PS_Charge)
			OwningPlayer->PS_Charge->DeactivateImmediate();
		bIsThrowing = true;
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

	TArray<ACharacter_B*> PlayersInCone;

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
	ACharacter_B* TargetPlayer = nullptr;

	switch (PlayersInCone.Num())
	{
	case 0:
		return false;
	case 1:
		TargetPlayer = PlayersInCone[0];
		break;
	default:
		PlayersInCone.Sort([&](const ACharacter_B& LeftSide, const ACharacter_B& RightSide)
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
void UThrowComponent_B::OneCharacterChanged()
{
	/// Finds all characters
	OtherPlayers.Empty();
	TArray<AActor*> TempArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_B::StaticClass(), TempArray);
	for (const auto& Player : TempArray)
	{
		if (Cast<ACharacter_B>(Player) == OwningPlayer)
			continue;
		OtherPlayers.Add(Cast<ACharacter_B>(Player));
	}

	/// Finds the holdcomponent.
	// Dette kan kanskje flyttes til BeginPlay ? Usikker på hvilke av komponentene som blir laget først
	if (!OwningPlayer) { BError("Can't find owningplayer!"); return; }

	HoldComponent = OwningPlayer->HoldComponent;
}

void UThrowComponent_B::Throw()
{
	if (!OwningPlayer)
	{
		BError("No OwningCharacter for ThrowComponent %s!", *GetNameSafe(this));
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

	/// Prepare item to be thrown
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(HoldComponent->GetHoldingItem());
	if (Interface)
	{
		Interface->Execute_Use(HoldComponent->GetHoldingItem());
	}
	if (OwningPlayer)
	{
		if (OwningPlayer->HoldComponent)
			OwningPlayer->HoldComponent->SetHoldingItem(nullptr);	//had a crash on this line before these checks
		else
			BError("No HoldComponent for player %f", *GetNameSafe(OwningPlayer));
		if (OwningPlayer->PS_Charge)
			OwningPlayer->PS_Charge->DeactivateImmediate();
		else
			BError("No PS_Charge for player %f", *GetNameSafe(OwningPlayer));
		OwningPlayer->SetState(EState::EWalking);
	}
	else
		BError("No OwningPlayer for hold component %f", *GetNameSafe(this));
	bIsCharging = false;
	bIsThrowing = false;
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
