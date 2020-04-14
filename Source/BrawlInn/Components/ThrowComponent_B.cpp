// Fill out your copyright notice in the Description page of Project Settings.

#include "ThrowComponent_B.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"

#include "DrawDebugHelpers.h"

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
	if (GameMode)
	{
		GameMode->SpawnCharacter_NOPARAM_D.AddUObject(this, &UThrowComponent_B::OnAnyCharacterChanged);
		GameMode->OnRespawnCharacter_D.AddUObject(this, &UThrowComponent_B::OnAnyCharacterChanged);
		GameMode->DespawnCharacter_NOPARAM_D.AddUObject(this, &UThrowComponent_B::OnAnyCharacterChanged);
	}
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

		GetWorld()->GetTimerManager().SetTimer(TH_DrinkTimer, this, &UThrowComponent_B::FinishDrinking, Useable->GetUseTime());
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
	if (HoldComponent)
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

void UThrowComponent_B::ThrowDrink()
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
	AUseable_B* Useable = Cast<AUseable_B>(HoldComponent->GetHoldingItem());
	if (Useable)
	{
		Useable->ThrowAway(OwningCharacter->GetActorRightVector());
	}

	if (OwningCharacter->GetChargeParticle())
		OwningCharacter->GetChargeParticle()->DeactivateImmediate();
	else
		BError("No PS_Charge for player %s", *GetNameSafe(OwningCharacter));

	OwningCharacter->SetIsCharging(false);
	bIsThrowing = false;

}

bool UThrowComponent_B::AimAssist(FVector& TargetPlayerLocation)
{
	if (OtherPlayers.Num() == 0)
		return false;

	FVector PlayerLocation = OwningCharacter->GetActorLocation();

	FVector PlayerForward = PlayerLocation + OwningCharacter->GetActorForwardVector() * AimAssistRange;
	PlayerForward.Z = 0;

	FVector PlayerToForward = PlayerForward - PlayerLocation;
	DrawDebugCone(GetWorld(), PlayerLocation, PlayerToForward.GetSafeNormal(), AimAssistRange, FMath::DegreesToRadians(AimAssistAngle), FMath::DegreesToRadians(AimAssistAngle), 16, FColor::Red, false, 5.f);
	DrawDebugCone(GetWorld(), PlayerLocation, PlayerToForward.GetSafeNormal(), AimAssistInnerRange, FMath::DegreesToRadians(AimAssistInnerAngle),0, 16, FColor::Red, false, 5.f);

	TArray<ACharacter_B*> PlayersInCone;
	TArray<ACharacter_B*> PlayersInInnerCone;

	for (const auto& OtherPlayer : OtherPlayers)
	{
		if (!IsValid(OtherPlayer) || (OtherPlayer->GetState() == EState::EFallen))
			continue;
		if (HoldComponent && (OtherPlayer == HoldComponent->GetHoldingItem()))
			continue;
		FVector OtherPlayerLocation = OtherPlayer->GetActorLocation();

		FVector PlayerToOtherPlayer = OtherPlayerLocation - PlayerLocation;

		if (PlayerToOtherPlayer.Size() > AimAssistRange)
			continue;

		//Inner range check
		if (PlayerToOtherPlayer.Size() < AimAssistInnerRange)
		{
			FVector ZeroZPlayerToForward = PlayerToForward;
			ZeroZPlayerToForward.Z = 0;
			FVector ZeroZPlayerToOtherPlayer = PlayerToOtherPlayer;
			ZeroZPlayerToOtherPlayer.Z = 0;

			float AngleA = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ZeroZPlayerToForward.GetSafeNormal(), ZeroZPlayerToOtherPlayer.GetSafeNormal())));
			if (AngleA <= AimAssistInnerAngle)
			{
				BWarn("Found Character in cone!");
				PlayersInInnerCone.Add(OtherPlayer);
			}
		}
		if (PlayersInInnerCone.Num() > 0)
			continue;
		//outer range check
		float AngleA = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(PlayerToForward.GetSafeNormal(), PlayerToOtherPlayer.GetSafeNormal())));
		if (AngleA <= AimAssistAngle)
		{
			BWarn("Found Character in cone!");
			PlayersInCone.Add(OtherPlayer);
		}
	}
	TArray<ACharacter_B*> ArrayToCheck = PlayersInCone;
	if (PlayersInInnerCone.Num() > 0)
		ArrayToCheck = PlayersInInnerCone;

	ACharacter_B* TargetPlayer = nullptr;
	switch (ArrayToCheck.Num())
	{
	case 0:
		return false;
	case 1:
		TargetPlayer = ArrayToCheck[0];
		break;
	default:
		ArrayToCheck.Sort([&](const ACharacter_B& LeftSide, const ACharacter_B& RightSide)
			{
				FVector A = LeftSide.GetActorLocation();
				A.Z = 0;
				FVector B = RightSide.GetActorLocation();
				B.Z = 0;
				float DistanceA = FVector::Dist(PlayerLocation, LeftSide.GetActorLocation());
				float DistanceB = FVector::Dist(PlayerLocation, RightSide.GetActorLocation());
				return DistanceA < DistanceB;
			});
		TargetPlayer = ArrayToCheck[0];
		break;
	}
	
	FVector TargetLocation = TargetPlayer->GetActorLocation() + FVector(0.f, 0.f, TargetPlayer->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	FVector ThrowDirection = TargetLocation - HoldComponent->GetHoldingItem()->GetActorLocation();
	TargetPlayerLocation = ThrowDirection.GetSafeNormal();

	return true;
}

void UThrowComponent_B::OnAnyCharacterChanged()
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
