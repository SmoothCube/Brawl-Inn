// Fill out your copyright notice in the Description page of Project Settings.

#include "ThrowComponent_B.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"

#include "BrawlInn.h"
#include "Characters/Character_B.h"
#include "Components/HoldComponent_B.h"
#include "System/GameModes/GameMode_B.h"
#include "System/GameInstance_B.h"

UThrowComponent_B::UThrowComponent_B(const FObjectInitializer& ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UThrowComponent_B::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacter_B>(GetOwner());

	GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->SpawnCharacter_NOPARAM_D.AddUObject(this, &UThrowComponent_B::OneCharacterChanged);
	GameMode->OnRespawnCharacter_D.AddUObject(this, &UThrowComponent_B::OneCharacterChanged);
	GameMode->DespawnCharacter_NOPARAM_D.AddUObject(this, &UThrowComponent_B::OneCharacterChanged);

	HoldComponent = OwningCharacter->HoldComponent;
}

void UThrowComponent_B::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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
		return false;
	}
	else if (!(OwningCharacter->GetState() == EState::EHolding))
	{
		BWarn("Wrong Player State");
		return false;
	}

	//BWarn("Trying Charge!");
	if (IsValid(OwningCharacter))
		OwningCharacter->bIsCharging = true;
	if (OwningCharacter && OwningCharacter->GetChargeParticle())
		OwningCharacter->GetChargeParticle()->Activate();
	return true;
}

void UThrowComponent_B::StartThrow()
{
	if (OwningCharacter->bIsCharging)
	{
		OwningCharacter->bIsCharging = false;
		if (OwningCharacter && OwningCharacter->GetChargeParticle())
			OwningCharacter->GetChargeParticle()->DeactivateImmediate();
		bIsThrowing = true;
	}
}

void UThrowComponent_B::Throw()
{
	if (!OwningCharacter)
	{
		BError("No OwningCharacter for ThrowComponent %s!", *GetNameSafe(this));
		return;
	}
	if (!HoldComponent)
	{
		BError("No HoldComponent for ThrowComponent");
		return;
	}
	if (!HoldComponent->IsHolding())
	{
		OwningCharacter->SetState(EState::EWalking);
		return;
	}

	/// Prepare item to be thrown
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(HoldComponent->GetHoldingItem());
	if (Interface)
	{
		Interface->Execute_Use(HoldComponent->GetHoldingItem());
	}
	if (IsValid(OwningCharacter))
	{
		if (IsValid(OwningCharacter->HoldComponent))
		{
			OwningCharacter->HoldComponent->SetHoldingItem(nullptr);
		}
		else
		{
			BError("No HoldComponent for player %f", *GetNameSafe(OwningCharacter));
		}
		if (OwningCharacter->GetChargeParticle())
			OwningCharacter->GetChargeParticle()->DeactivateImmediate();
		else
			BError("No PS_Charge for player %f", *GetNameSafe(OwningCharacter));
		OwningCharacter->SetState(EState::EWalking);
	}
	else
		BError("No OwningPlayer for hold component %f", *GetNameSafe(this));
	OwningCharacter->bIsCharging = false;
	bIsThrowing = false;
}

bool UThrowComponent_B::IsThrowing() const
{
	return bIsThrowing;
}

bool UThrowComponent_B::AimAssist(FVector& TargetPlayerLocation)
{
	if (OtherPlayers.Num() == 0)
		return false;

	FVector PlayerLocation = OwningCharacter->GetActorLocation();
	PlayerLocation.Z = 0;
	FVector PlayerForward = PlayerLocation + OwningCharacter->GetActorForwardVector() * AimAssistRange;
	PlayerForward.Z = 0;

	FVector PlayerToForward = PlayerForward - PlayerLocation;

	TArray<ACharacter_B*> PlayersInCone;

	for (const auto& OtherPlayer : OtherPlayers)
	{
		if (!IsValid(OtherPlayer) || (OtherPlayer->GetState() == EState::EFallen))
			continue;
		if (HoldComponent && (OtherPlayer == HoldComponent->GetHoldingItem()))
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
		if (Cast<ACharacter_B>(Player) == OwningCharacter)
			continue;
		OtherPlayers.Add(Cast<ACharacter_B>(Player));
	}

	/// Finds the holdcomponent.
	// Dette kan kanskje flyttes til BeginPlay ? Usikker på hvilke av komponentene som blir laget først
	if (!OwningCharacter) { BError("Can't find owningplayer!"); return; }

	HoldComponent = OwningCharacter->HoldComponent;
}

bool UThrowComponent_B::IsReady() const
{
	if (!GameMode)
	{
		BWarn("No game mode!");
		return false;
	}
	if (!OwningCharacter)
	{
		BWarn("No Owning OwningCharacter");
		return false;
	}
	else if (!HoldComponent)
	{
		BWarn("No Hold Component!");
		return false;
	}
	return true;
}
