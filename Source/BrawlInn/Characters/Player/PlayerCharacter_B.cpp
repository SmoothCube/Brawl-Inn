// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter_B.h"
#include "Components/CapsuleComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
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
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
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

	PS_ChiliBrew = CreateDefaultSubobject<UNiagaraComponent>("ChiliBrewParticle");
	PS_ChiliBrew->SetupAttachment(GetMesh());
	PS_ChiliBrew->SetRelativeLocation(FVector(0.f, -14.5f, 80.f));

	//variables overridden from ACharacter_B
	SpecialMaterialIndex = 0;
	ForceSocketName = "spine2_export_C_jnt";
	PunchesToStun = 4;
	bCanBeHeld = false;
	GetCapsuleComponent()->SetCapsuleRadius(75.f);

	Charge1ThrowStrength = 400000.f;
	Charge2ThrowStrength = 1000000.f;
	Charge3ThrowStrength = 2000000.f;

	HoldOffset = FVector(0, 0, 500);
	HoldingDrinkOffset = FVector(-49.212128, -8.693832, -0.485261);
}

void APlayerCharacter_B::BeginPlay()
{
	Super::BeginPlay();

	ScoreTextWidget = CreateWidget<UScoreText_B>(GetWorld(), BP_ScoreTextWidget);
	ScoreTextWidgetComponent->SetWidget(ScoreTextWidget);
	ScoreTextWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	PS_ChiliBrew->Deactivate();

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	checkf(GameInstance, TEXT("%s can't find the GameInstance_B! ABORT"), *GetNameSafe(this));

	if (GameInstance->ShouldUseSpreadSheets())
	{
		UDataTable_B* Table = NewObject<UDataTable_B>();
		Table->LoadCSVFile(FScoreTable::StaticStruct(), "DefaultScoreValues.csv");
		FallScoreAmount = Table->GetRow<FScoreTable>("Fall")->Value;
		FellOutOfWorldScoreAmount = Table->GetRow<FScoreTable>("FellOutOfWorld")->Value;
		PowerupKnockdownScoreAmount = Table->GetRow<FScoreTable>("PowerupKnockdown")->Value;
		DashThroughScoreValue = Table->GetRow<FScoreTable>("DashThroughScoreValue")->Value;
	}

	GetWorld()->GetTimerManager().SetTimer(TH_MakeNoiseTimer, [&]()
		{
			NoiseEmitterComponent->MakeNoise(this, 1.f, GetActorLocation());
		}, 0.5f, true);
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

USoundCue* APlayerCharacter_B::GetChiliBrewSound() const
{
	return ChiliBrewSound;
}

void APlayerCharacter_B::FellOutOfWorld(const UDamageType& dmgType)
{
	if (HoldComponent)
		HoldComponent->Drop();
	Super::FellOutOfWorld(dmgType);
}

void APlayerCharacter_B::Die()
{
	Super::Die();
	if (DirectionIndicatorPlane)
		DirectionIndicatorPlane->DestroyComponent();

	UGameplayStatics::ApplyDamage(this, FellOutOfWorldScoreAmount, LastHitBy, LastHitBy, UOutOfWorld_DamageType_B::StaticClass());

	PlayerController->TryRespawn(RespawnDelay);
	PlayerController->UnPossess();
}

void APlayerCharacter_B::Fall(FVector MeshForce, float RecoveryTime, bool bPlaySound)
{
	Super::Fall(MeshForce, RecoveryTime, true);
	bCanBeHeld = true;
	if (PlayerController)
		PlayerController->PlayControllerVibration(1.f, 0.5f, true, true, true, true);
	if (IsValid(DirectionIndicatorPlane))
		DirectionIndicatorPlane->SetScalarParameterValueOnMaterials("Health", PunchesToStun);

	if (IsValid(HighShatterSound) && bPlaySound)
	{
		float Volume = 1.f;
		GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (GameInstance)
			Volume *= GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume();
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			HighShatterSound,
			GetActorLocation(),
			Volume
			);
	}
}

void APlayerCharacter_B::StandUp()
{
	Super::StandUp();
	bCanBeHeld = false;
	if (DirectionIndicatorPlane)
	{
		DirectionIndicatorPlane->SetScalarParameterValueOnMaterials("Health", StunAmount); //Had a crash here -e
		DirectionIndicatorPlane->SetHiddenInGame(false);
	}
	MakeInvulnerable(InvulnerabilityTime);
	CurrentHoldTime = 0.f;
}

void APlayerCharacter_B::PickedUp_Implementation(ACharacter_B* Player)
{
	Super::PickedUp_Implementation(Player);

	DirectionIndicatorPlane->SetHiddenInGame(true);
}

void APlayerCharacter_B::Dropped_Implementation()
{
	Super::Dropped_Implementation();
	DirectionIndicatorPlane->SetHiddenInGame(true);
	CurrentHoldTime = 0.f;
}

void APlayerCharacter_B::BreakFreeButtonMash()
{
	if (BreakFreeAnimationBlend <= 0.f)
		BreakFreeAnimationBlend = 0.5;
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
		HoldingCharacter->SetIsCharging(false); //TODO set charge level? 
		HoldingCharacter->AddStun(PunchesToStun - 1);
		HoldingCharacter = nullptr;
		AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->AddCameraFocusPoint(this);
		}
	}

	StandUp();

	CurrentHoldTime = 0.f;
}

float APlayerCharacter_B::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	BLog("%f", DamageAmount);
	if (IsInvulnerable() && !(DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UOutOfWorld_DamageType_B::StaticClass())))
		return 0;

	BLog("%s Taking Damage. Causer: %s", *GetNameSafe(PlayerController), *GetNameSafe(EventInstigator));
	if (GameInstance)
	{
		GameInstance->PlayImpactCameraShake(GetActorLocation());
		const float Trauma = DamageAmount / 100;
		if (PlayerController)
			PlayerController->PlayControllerVibration(FMath::Square(Trauma), 0.3, true, true, true, true);
	}

	if (EventInstigator == PlayerController)
		return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	float Score = DamageAmount;
	AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode && GameMode->ShouldUseScoreMultiplier())
	{
		auto LeadingControllers = GameMode->GetLeadingPlayerController();

		if ((LeadingControllers.Num() == 1) && (PlayerController == LeadingControllers[0]))
			Score *= 2;
	}

	if (DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UOutOfWorld_DamageType_B::StaticClass()))
	{
		if (LastHitBy) // Hit by someone before falling out of the world!
		{
			APlayerController_B* OtherPlayerController = Cast<APlayerController_B>(LastHitBy);
			if (OtherPlayerController)
			{
				OtherPlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddScore(Score);
			}
		}
		else
		{
			PlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddScore(-Score);
		}
	}
	else
	{
		APlayerController_B* OtherPlayerController = Cast<APlayerController_B>(EventInstigator);
		if (OtherPlayerController)
		{
			OtherPlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddScore(Score);
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
	if (ScoreValues.LastScoreAdded <= 0)
		return;

	check(IsValid(GameInstance));

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ScoreParticle, GetActorLocation());

	const float Volume = 1.f * GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume();
	const float Pitch = FMath::RandRange(0.7f, 1.2f);

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ScoreSound, GetActorLocation(), Volume, Pitch);

	ScoreTextWidget->DisplayScore(ScoreValues.LastScoreAdded);

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

void APlayerCharacter_B::SetChargeLevel(EChargeLevel chargeLevel)
{
	BWarn("Setting Charge Level to %d!", (int)chargeLevel);
	Super::SetChargeLevel(chargeLevel);
	float VibrationStrength = 0.f;
	switch (chargeLevel)
	{
	case EChargeLevel::EChargeLevel1:
		VibrationStrength = 0.5f;
		break;
	case EChargeLevel::EChargeLevel2:
		VibrationStrength = 0.7f;
		break;
	case EChargeLevel::EChargeLevel3:
		VibrationStrength = 1.f;
		break;
	}
	PlayerController->PlayControllerVibration(VibrationStrength, 0.1f, true, true, true, true);
}

void APlayerCharacter_B::AddStun(const int Strength)
{
	Super::AddStun(Strength);
	DirectionIndicatorPlane->SetScalarParameterValueOnMaterials("Health", StunAmount);

	float Volume = 1.f;
	GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance)
		Volume *= GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume();

	switch (StunAmount)
	{
	case 0:
		break;
	case 1:
		if (LowShatterSound)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), LowShatterSound, GetActorLocation(), Volume);
		break;
	case 2:
	case 3:
		if (MidShatterSound)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), MidShatterSound, GetActorLocation(), Volume);
		break;
	case 4:
		break;
	}
}

void APlayerCharacter_B::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PlayerController = Cast<AGamePlayerController_B>(NewController);

	if (!PlayerController)
		return;

	DisplayScoreVisualsHandle = PlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->OnScoreValuesChanged.AddUObject(this, &APlayerCharacter_B::DisplayScoreVisuals);

	PunchComponent->OnPunchHit_D.AddLambda([&]() //Keeps crashing here after compile -E
		{
			PlayerController->PlayControllerVibration(0.2f, 0.3f, true, true, true, true);
		});

}

void APlayerCharacter_B::UnPossessed()
{
	AGamePlayerController_B* OldController = Cast<AGamePlayerController_B>(Controller);
	if (OldController)
	{
		OldController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->OnScoreValuesChanged.Remove(DisplayScoreVisualsHandle);
	}

	Super::UnPossessed();

}

//For dashing through characters
void APlayerCharacter_B::OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnCapsuleOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ACharacter_B* OtherCharacter = Cast<ACharacter_B>(OtherActor);
	if (!IsValid(OtherCharacter) || OtherCharacter->GetState() == EState::EFallen)
		return;

	int DamageAmount = 5;

	UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(OtherComp);
	if (IsValid(Capsule))
	{
		if (GetState() == EState::EPoweredUp)
		{
			OtherCharacter->GetCharacterMovement()->Velocity = (OtherCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal() * OtherCharacter->PowerupPushStrength;

			DamageAmount = PowerupKnockdownScoreAmount;
		}
		else if (IsValid(PunchComponent) && PunchComponent->GetIsDashing() && !OtherCharacter->IsInvulnerable())
		{
			OtherCharacter->GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity * (-PunchComponent->DashPushPercentage);

			DamageAmount = DashThroughScoreValue;
		}
		UGameplayStatics::ApplyDamage(OtherCharacter, DamageAmount, PlayerController, this, UDamageType::StaticClass());
	}
}
