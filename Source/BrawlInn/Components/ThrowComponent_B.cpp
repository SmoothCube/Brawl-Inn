// Fill out your copyright notice in the Description page of Project Settings.

#include "ThrowComponent_B.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"

#include "BrawlInn.h"
#include "Characters/Character_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/PunchComponent_B.h"
#include "System/GameModes/GameMode_B.h"
#include "Items/Useable_B.h"

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

void UThrowComponent_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void UThrowComponent_B::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UThrowComponent_B::TryStartCharge()
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

	AUseable_B* Useable = Cast<AUseable_B>(HoldComponent->GetHoldingItem());
	if (Useable)
	{
		StartDrinking();

		GetWorld()->GetTimerManager().SetTimer(TH_DrinkTimer,this, &UThrowComponent_B::FinishDrinking,Useable->GetUseTime());
	}
	else if (OwningCharacter->GetChargeParticle())
	{
		OwningCharacter->SetIsCharging(true);
		OwningCharacter->GetChargeParticle()->Activate();
	}

	return true;
}

void UThrowComponent_B::StartThrow()
{
	if(HoldComponent)
		if (HoldComponent->GetHoldingItem()->IsA(AUseable_B::StaticClass()))
			return;
	if (OwningCharacter->IsCharging())
	{
		OwningCharacter->SetIsCharging(false);
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
	OwningCharacter->SetState(EState::EWalking);
	/// Prepare item to be thrown
	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(HoldComponent->GetHoldingItem());
	if (Interface)
	{
		Interface->Execute_Use(HoldComponent->GetHoldingItem());
	}

	HoldComponent->SetHoldingItem(nullptr);

	if (OwningCharacter->GetChargeParticle())
		OwningCharacter->GetChargeParticle()->DeactivateImmediate();
	else
		BError("No PS_Charge for player %s", *GetNameSafe(OwningCharacter));

	OwningCharacter->SetIsCharging(false);
	bIsThrowing = false;
}

bool UThrowComponent_B::IsThrowing() const
{
	return bIsThrowing;
}

bool UThrowComponent_B::IsDrinking()
{
	return bIsDrinking;
}

void UThrowComponent_B::StartDrinking()
{
	bIsDrinking = true;
	//remove all control from player for a while
	OwningCharacter->SetCanMove(false);
	if (OwningCharacter->PunchComponent)
	{
		OwningCharacter->PunchComponent->SetCanPunch(false);
		OwningCharacter->PunchComponent->SetCanDash(false);
	}
}

void UThrowComponent_B::FinishDrinking()
{
	OwningCharacter->MakeInvulnerable(-1.f, false);
	bIsDrinking = false;
	bIsDrinkingFinished = true;
}

void UThrowComponent_B::CancelDrinking()
{
	bIsDrinking = false;
	OwningCharacter->SetCanMove(true);
	if (OwningCharacter->PunchComponent)
	{
		OwningCharacter->PunchComponent->SetCanPunch(true);
		OwningCharacter->PunchComponent->SetCanDash(true);
	}
}

bool UThrowComponent_B::IsDrinkingFinished()
{
	return bIsDrinkingFinished;
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
	// Dette kan kanskje flyttes til BeginPlay ? Usikker p� hvilke av komponentene som blir laget f�rst
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
