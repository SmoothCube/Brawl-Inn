// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter_B.h"
#include "Components/CapsuleComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
#include "WidgetComponent.h"

#include "BrawlInn.h"
#include "Characters/Player/GamePlayerController_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/PunchComponent_B.h"
#include "Hazards/BounceActor/BounceActor_B.h"
#include "Items/Throwable_B.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/AI/AICharacter_B.h"
#include "Characters/AI/IdleAICharacter_B.h"
#include "System/Camera/GameCamera_B.h"
#include "System/DamageTypes/OutOfWorld_DamageType_B.h"
#include "System/DataTable_B.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/MainGameMode_B.h"
#include "System/Structs/ScoreLookupTable.h"
#include "System/SubSystems/ScoreSubSystem_B.h"
#include "UI/Widgets/ScoreText_B.h"

APlayerCharacter_B::APlayerCharacter_B()
{
	DirectionIndicatorPlane = CreateDefaultSubobject<UStaticMeshComponent>("Direction Indicator Plane");
	DirectionIndicatorPlane->SetupAttachment(RootComponent);
	DirectionIndicatorPlane->SetRelativeLocation(FVector(20, 0, -70));
	DirectionIndicatorPlane->SetRelativeRotation(FRotator(0, 90, 0));
	DirectionIndicatorPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DirectionIndicatorPlane->SetRelativeScale3D(FVector(3.327123, 3.327123, 1));

	ScoreTextWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("ScoreTextWidgetComponent");
	ScoreTextWidgetComponent->SetupAttachment(GetRootComponent());

	NoiseEmitterComponent = CreateDefaultSubobject<UPawnNoiseEmitterComponent>("NoiseEmitterComponent");

	ChiliBrewSoundComp = CreateDefaultSubobject<UAudioComponent>("ChiliBrewSoundComp");

	PS_ChiliBrew = CreateDefaultSubobject<UNiagaraComponent>("ChiliBrewParticle");
	PS_ChiliBrew->SetupAttachment(GetMesh());
	PS_ChiliBrew->SetRelativeLocation(FVector(0.f, -14.5f, 80.f));

	PS_CandleFlame = CreateDefaultSubobject<UNiagaraComponent>("CandleFlameParticle");
	PS_CandleFlame->SetupAttachment(GetMesh(), "neckEnd_export_C_jnt");
	PS_CandleFlame->SetRelativeLocation(FVector(0.f, -33.f, 8.f));
	PS_CandleFlame->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));

	//variables overridden from ACharacter_B
	SpecialMaterialIndex = 0;
	PunchesToStun = 4;
	GetCapsuleComponent()->SetCapsuleRadius(75.f);

	ThrowStrength = 1000000.f;

	FallRecoveryTime = 2.f;
	ThrowRecoveryTime = 1.f;
	PickupWeight = 4.f;
}

void APlayerCharacter_B::BeginPlay()
{
	Super::BeginPlay();

	ScoreTextWidget = CreateWidget<UScoreText_B>(GetWorld(), BP_ScoreTextWidget);
	ScoreTextWidgetComponent->SetWidget(ScoreTextWidget);
	ScoreTextWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	PS_ChiliBrew->Deactivate();
	ChiliBrewSoundComp->Stop();

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	checkf(GameInstance, TEXT("%s can't find the GameInstance_B! ABORT"), *GetNameSafe(this));

	if (GameInstance->ShouldUseSpreadSheets())
	{
		UDataTable_B* Table = NewObject<UDataTable_B>();
		Table->LoadCSVFile(FScoreTable::StaticStruct(), "DefaultScoreValues.csv");
		FallScoreAmount = Table->GetRow<FScoreTable>("Knockout")->Value;
		FellOutOfWorldScoreAmount = Table->GetRow<FScoreTable>("ThrowingPlayerOutOfWorld")->Value;
		SuicideScoreAmount = Table->GetRow<FScoreTable>("Suicide")->Value;
		PowerupKnockbackScoreAmount = Table->GetRow<FScoreTable>("PowerupKnockback")->Value;
		DashThroughScoreValue = Table->GetRow<FScoreTable>("DashThroughScoreValue")->Value;
	}

	GetWorld()->GetTimerManager().SetTimer(TH_MakeNoiseTimer, [&]()
		{
			NoiseEmitterComponent->MakeNoise(this, 1.f, GetActorLocation());
		}, 0.5f, true);

	PunchComponent->OnHitPlayerPunch_D.AddUObject(this, &APlayerCharacter_B::OnPunchHit);
}

void APlayerCharacter_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	GetWorld()->GetTimerManager().ClearTimer(TH_MakeNoiseTimer);
}

void APlayerCharacter_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetState() == EState::EPoweredUp)
	{
		HandleMovementPoweredUp(DeltaTime);
	}
	else if (GetState() == EState::EBeingHeld)
	{
		CurrentHoldTime += DeltaTime;
		if (CurrentHoldTime >= MaxHoldTime)
			BreakFree();
	}

	BreakFreeAnimationBlend -= 0.5 * DeltaTime;
	if (BreakFreeAnimationBlend < 0.f)
		BreakFreeAnimationBlend = 0.f;
}

void APlayerCharacter_B::HandleMovementPoweredUp(float DeltaTime)
{
	//Normalizes to make sure we dont accelerate faster diagonally, but still want to allow for slower movement.
	if (InputVector.SizeSquared() >= 1.f)
		InputVector.Normalize();

	if (InputVector.SizeSquared() > 0)
	{
		if (GameCamera)
		{
			AddMovementInput(GameCamera->GetActorForwardVector(), InputVector.X);
			AddMovementInput(GameCamera->GetActorRightVector(), InputVector.Y);
		}

		const FVector Vec = GameCamera->GetActorForwardVector().ToOrientationRotator().RotateVector(InputVector);
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), Vec.ToOrientationRotator(), DeltaTime, RotationInterpSpeed));
	}
	else
	{
		const FVector Vec = GameCamera->GetActorForwardVector().ToOrientationRotator().UnrotateVector(GetActorRotation().Vector());
		AddMovementInput(GameCamera->GetActorForwardVector(), Vec.X);
		AddMovementInput(GameCamera->GetActorRightVector(), Vec.Y);
	}
}

UStaticMeshComponent* APlayerCharacter_B::GetDirectionIndicatorPlane() const
{
	return DirectionIndicatorPlane;
}

UNiagaraComponent* APlayerCharacter_B::GetChiliBrewParticle() const
{
	return PS_ChiliBrew;
}

UAudioComponent* APlayerCharacter_B::GetChiliBrewSound() const
{
	return ChiliBrewSoundComp;
}

void APlayerCharacter_B::FellOutOfWorld(const UDamageType& dmgType)
{
	if (HoldComponent)
		HoldComponent->Drop();

	if (bIsAlive)
		Die();

	Super::FellOutOfWorld(dmgType);
}

void APlayerCharacter_B::Die()
{
	Super::Die();

	PlayApplauseOnNearbyNPCs();
	
	if (DirectionIndicatorPlane)
		DirectionIndicatorPlane->DestroyComponent();

	//playEnd
	if (ChiliBrewSoundComp && ChiliBrewSoundComp->IsPlaying())
	{
		if (ChiliBrewEndSound)
		{
			ChiliBrewSoundComp->SetSound(ChiliBrewEndSound);
			ChiliBrewSoundComp->Play();
		}
		else
		{
			ChiliBrewSoundComp->Stop();

		}
	}

	PS_ChiliBrew->Deactivate();
	PS_CandleFlame->Deactivate();
	if (PunchComponent->OnHitPlayerPunch_D.IsBoundToObject(this))
		PunchComponent->OnHitPlayerPunch_D.RemoveAll(this);

	UGameplayStatics::ApplyDamage(this, FellOutOfWorldScoreAmount, LastHitBy, LastHitBy, UOutOfWorld_DamageType_B::StaticClass());

	if (PlayerController)
	{
		PlayerController->TryRespawn(RespawnDelay);
		PlayerController->UnPossess();
	}

	GetWorld()->GetTimerManager().SetTimer(TH_Despawn, this, &APlayerCharacter_B::BeginDespawn, GetWorld()->GetDeltaSeconds(), true, TimeBeforeDespawn);
	GetWorld()->GetTimerManager().SetTimer(TH_Destroy, this, &APlayerCharacter_B::StartDestroy, TimeBeforeDespawn + 2.f, false);

}

void APlayerCharacter_B::PlayApplauseOnNearbyNPCs()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIdleAICharacter_B::StaticClass(), OutActors);

	TArray<AActor*> ActorsToKeep;
	
	for(auto Actor : OutActors)
	{
		if (GetDistanceTo(Actor) <= MaxClapDistanceToNPC)
			ActorsToKeep.Add(Actor);
	}

	for(auto Actor : ActorsToKeep)
	{
		auto NPC = Cast<AIdleAICharacter_B>(Actor);
		if(NPC)
		{
			NPC->PlayApplauseMontage();
		}
	}
}

void APlayerCharacter_B::BeginDespawn()
{
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void APlayerCharacter_B::StartDestroy()
{
	Destroy();
}

void APlayerCharacter_B::Fall(FVector MeshForce, float RecoveryTime, bool bPlaySound)
{
	Super::Fall(MeshForce, RecoveryTime, true);

	if (PlayerController)
		PlayerController->PlayControllerVibration(1.f, 0.5f, true, true, true, true);
	if (IsValid(DirectionIndicatorPlane))
		DirectionIndicatorPlane->SetScalarParameterValueOnMaterials("Health", PunchesToStun);

	if (IsValid(HighShatterSound) && bPlaySound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			HighShatterSound,
			GetActorLocation(),
			1.f
		);
	}
}

void APlayerCharacter_B::StandUp()
{
	if (!bIsAlive)
		return;

	Super::StandUp();

	if (bShouldStand)
		return;

	if (DirectionIndicatorPlane)
	{
		DirectionIndicatorPlane->SetScalarParameterValueOnMaterials("Health", StunAmount); //Had a crash here -e
		DirectionIndicatorPlane->SetHiddenInGame(false);
	}
	MakeInvulnerable(InvulnerabilityTime);
	CurrentHoldTime = 0.f;

	if (PlayerController && PlayerController->IsPunchChargeInputHeld())
		PlayerController->TryStartPunchCharge();
}

void APlayerCharacter_B::PickedUp_Implementation(ACharacter_B* Player)
{
	Super::PickedUp_Implementation(Player);

	DirectionIndicatorPlane->SetHiddenInGame(true);
}

void APlayerCharacter_B::Dropped_Implementation()
{
	Super::Dropped_Implementation();
	//	DirectionIndicatorPlane->SetHiddenInGame(true);
	CurrentHoldTime = 0.f;
}

void APlayerCharacter_B::Use_Implementation()
{
	SetLastHitBy(HoldingCharacter->GetController());

	Super::Use_Implementation();

}

void APlayerCharacter_B::BreakFreeButtonMash()
{
	if (BreakFreeAnimationBlend <= 0.f)
		BreakFreeAnimationBlend = 0.8;
	BreakFreeAnimationBlend += 0.1f;
	CurrentHoldTime += HoldTimeDecreasePerButtonMash;
	PlayerController->PlayControllerVibration(0.8f, 0.1f, true, true, true, true);
}

void APlayerCharacter_B::BreakFree()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorRotation(FRotator(0, 0, 0));
	MakeInvulnerable(1.f);
	if (HoldingCharacter)
	{
		//Moves forward a bit before turning on collisions, so we dont fly to hell because we overlap with the other player
		AddActorLocalOffset(HoldingCharacter->GetActorForwardVector() * 100);
		HoldingCharacter->HoldComponent->SetHoldingItem(nullptr);
		HoldingCharacter->SetState(EState::EWalking);
		HoldingCharacter->SetIsCharging(false);
		HoldingCharacter->AddStun(PunchesToStun - 1);
		HoldingCharacter = nullptr;
		AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->AddCameraFocusPoint(this);
		}
	}
	Fall(FVector(0.f), -1.f, false);
	StandUp();

	CurrentHoldTime = 0.f;
}

float APlayerCharacter_B::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsInvulnerable() && !(DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UOutOfWorld_DamageType_B::StaticClass())))
		return 0;

	if (GameInstance)
	{
		GameInstance->PlayImpactCameraShake(GetActorLocation());
		if (PlayerController)
		{
			checkf(PunchesToStun != 0, TEXT("Division by zero!"));
			const float Trauma = static_cast<float>(StunAmount) / static_cast<float>(PunchesToStun);
			PlayerController->PlayControllerVibration(FMath::Square(Trauma), 0.3, true, true, true, true);
		}
	}

	if (EventInstigator == PlayerController)
		return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);


	float Score = DamageAmount;
	bool bIsMultiplied = false;
	bool bIsAgainstLeader = false;
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode && GameMode->ShouldUseScoreMultiplier())
	{
		bIsMultiplied = GameMode->MultipleScoreIsActivated();
		if (bIsMultiplied)
			Score *= 2;

		auto LeadingControllers = GameMode->GetLeadingPlayerController();

		if ((LeadingControllers.Num() == 1) && (PlayerController == LeadingControllers[0]))
		{
			Score *= 2;
			bIsAgainstLeader = true;
		}
	}

	//Falling out of the map
	if (DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UOutOfWorld_DamageType_B::StaticClass()))
	{
		if (PlayerController) //Something has gone wrong here before
		{
			if (PlayerController->GetLocalPlayer())
			{
				if (PlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>())
					PlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddStats(1, OutOfMapDeaths);
				else
					BError("Couldnt find ScoreSubSystem for actor %s! Something is wrong!", *GetNameSafe(this));
			}
			else
				BError("Couldnt find LocalPlayer for actor %s! Something is wrong!", *GetNameSafe(this));
		}
		else
			BError("Couldnt find PlayerController for actor %s! Something is wrong!", *GetNameSafe(this));


		if (LastHitBy) // Hit by someone before falling out of the world!
		{
			APlayerController_B* OtherPlayerController = Cast<APlayerController_B>(LastHitBy);
			if (OtherPlayerController)
				OtherPlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddPoints(Score, bIsMultiplied, bIsAgainstLeader);
		}
		else
		{
			PlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddPoints(SuicideScoreAmount, false, false);
		}
	}
	else //Everything else
	{
		APlayerController_B* OtherPlayerController = Cast<APlayerController_B>(EventInstigator);
		if (OtherPlayerController)
		{
			OtherPlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddPoints(Score, bIsMultiplied, bIsAgainstLeader);

			if (DamageCauser->IsA(ABounceActor_B::StaticClass()))
			{
				OtherPlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddStats(1, EScoreValueTypes::BarrelsHit);
			}
			else if (DamageCauser->IsA(AThrowable_B::StaticClass()))
			{
				OtherPlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddStats(1, EScoreValueTypes::ThrowablesHit);
			}
		}
	}

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	SetLastHitBy(EventInstigator);

	return DamageAmount;
}

void APlayerCharacter_B::SetLastHitBy(AController* EventInstigator)
{
	if (LastHitBy == EventInstigator)
	{
		GetWorld()->GetTimerManager().SetTimer(LastHitByTimer_TH, [&]() {
			LastHitBy = nullptr;
			}, 7.f, false);
	}
	else
	{
		LastHitBy = EventInstigator;
	}
}

void APlayerCharacter_B::DisplayScoreVisuals(const FScoreValues ScoreValues)
{

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ScoreParticle, GetActorLocation());

	const float Pitch = FMath::RandRange(0.7f, 1.2f);

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ScoreSound, GetActorLocation(), 1.f, Pitch);

	ScoreTextWidget->DisplayScore(ScoreValues.LastScoreAdded, ScoreValues.bIsMultiplied, ScoreValues.bIsAgainstLeader, PlayerController->GetPlayerInfo().CharacterVariant.TextColor);

	ScoreParticleTimerStart();
}

void APlayerCharacter_B::OnScoreParticleTimerUpdate(const float Value)
{
	FVector Location = ScoreTextWidgetComponent->GetRelativeLocation();
	Location.Z = Value;
	ScoreTextWidgetComponent->SetRelativeLocation(Location);
}

void APlayerCharacter_B::OnScoreParticleTimerFinished()
{
	check(IsValid(ScoreTextWidget));

	ScoreTextWidget->HideScore();
}

void APlayerCharacter_B::OnPunchHit()
{
	PlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddStats(1, EScoreValueTypes::PunchesHit);
	BLog("Punch hit: %i", PlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().PunchesHit);
}

void APlayerCharacter_B::SetChargeLevel(EChargeLevel chargeLevel)
{
	Super::SetChargeLevel(chargeLevel);
	float VibrationStrength = 0.f;
	float VibrationLength = 0.f;
	switch (chargeLevel)
	{

	case EChargeLevel::EChargeLevel2:
		VibrationStrength = 0.7f;
		VibrationLength = 0.5f;
		if (PlayerController && !PlayerController->IsPunchChargeInputHeld())
			PlayerController->TryEndPunchCharge();
		break;
	case EChargeLevel::EChargeLevel3:
		VibrationStrength = 0.7f;
		VibrationLength = -1.f;
		break;
	}
	if (PlayerController)
		PlayerController->PlayControllerVibration(VibrationStrength, VibrationLength, true, true, true, true);
}

void APlayerCharacter_B::AddStun(const int Strength)
{
	Super::AddStun(Strength);
	DirectionIndicatorPlane->SetScalarParameterValueOnMaterials("Health", StunAmount);

	switch (StunAmount)
	{
	case 0:
		break;
	case 1:
		if (LowShatterSound)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), LowShatterSound, GetActorLocation(), 1.f);
		break;
	case 2:
	case 3:
		if (MidShatterSound)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), MidShatterSound, GetActorLocation(), 1.f);
		break;
	case 4:
		break;
	}
}

AGamePlayerController_B* APlayerCharacter_B::GetPlayerController() const
{
	return PlayerController;
}

void APlayerCharacter_B::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PlayerController = Cast<AGamePlayerController_B>(NewController);

	if (!PlayerController)
		return;
	DisplayScoreVisualsHandle = PlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->OnScoreChanged.AddUObject(this, &APlayerCharacter_B::DisplayScoreVisuals);
}

void APlayerCharacter_B::UnPossessed()
{
	AGamePlayerController_B* OldController = Cast<AGamePlayerController_B>(Controller);
	if (OldController)
	{
		OldController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->OnScoreChanged.Remove(DisplayScoreVisualsHandle);
	}
	PlayerController = nullptr;

	Super::UnPossessed();

}

//For dashing through characters
void APlayerCharacter_B::OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnCapsuleOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ACharacter_B* OtherCharacter = Cast<ACharacter_B>(OtherActor);
	if (!IsValid(OtherCharacter) || OtherCharacter->GetState() == EState::EFallen || OtherCharacter == HoldingCharacter)
		return;

	UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(OtherComp);
	if (IsValid(Capsule) && Capsule == OtherActor->GetRootComponent())
	{
		int DamageAmount = 5;
		FVector Direction = FVector::ZeroVector;
		if (GetState() == EState::EPoweredUp)
		{
			BWarn("Powerup Overlap! %s", *GetNameSafe(OtherActor));
			Direction = ((OtherCharacter->GetActorLocation()) - GetActorLocation());
			Direction.Z = 0;
			Direction.Normalize();
			Direction = Direction.RotateAngleAxis(PowerupUpwardsAngle, FVector::CrossProduct(Direction, FVector(0, 0, 1.f)).GetSafeNormal());

			Direction *= OtherCharacter->PowerupPushStrength;
			OtherCharacter->AddStun();
			DamageAmount = PowerupKnockbackScoreAmount;
			if (OtherCharacter->GetStunAmount() >= OtherCharacter->GetPunchesToStun())
			{
				OtherCharacter->CheckFall(Direction);
			}
		}
		else if (IsValid(PunchComponent) && PunchComponent->GetIsDashing() && !OtherCharacter->IsInvulnerable())
		{
			Direction = -GetCharacterMovement()->Velocity.GetSafeNormal();
			Direction.Z = 0;
			Direction.Normalize();
			Direction = Direction.RotateAngleAxis(PunchComponent->DashPushUpwardsAngle, FVector::CrossProduct(Direction, FVector(0, 0, 1.f)).GetSafeNormal());

			Direction *= PunchComponent->DashPushStrength;

			DamageAmount = DashThroughScoreValue;

			const auto AICharacter = Cast<AAICharacter_B>(OtherCharacter);
			const auto IdleAICharacter = Cast<AIdleAICharacter_B>(OtherCharacter);
			if (AICharacter || IdleAICharacter)
			{
				OtherCharacter->CheckFall(Direction);
			}
		}
		OtherCharacter->GetCharacterMovement()->AddImpulse(Direction, false);
		UGameplayStatics::ApplyDamage(OtherCharacter, DamageAmount, PlayerController, this, UDamageType::StaticClass());
	}
}

void APlayerCharacter_B::OnCapsuleOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
