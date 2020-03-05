// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter_B.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/LocalPlayer.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"

#include "BrawlInn.h"
#include "System/DataTable_B.h"
#include "System/Structs/ScoreLookupTable.h"
#include "Characters/Player/PlayerController_B.h"
#include "Components/HealthComponent_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/HoldComponent_B.h"
#include "UI/Game/HealthWidget_B.h"
#include "System/DamageTypes/Stool_DamageType_B.h"
#include "System/DamageTypes/Fall_DamageType_B.h"
#include "System/DamageTypes/OutOfWorld_DamageType_B.h"
#include "System/GameInstance_B.h"
#include "System/GameModes/MainGameMode_B.h"
#include "System/SubSystems/ScoreSubSystem_B.h"
#include "Materials/MaterialInstanceDynamic.h"

APlayerCharacter_B::APlayerCharacter_B()
{
	DirectionIndicatorPlane = CreateDefaultSubobject<UStaticMeshComponent>("Direction Indicator Plane");
	DirectionIndicatorPlane->SetupAttachment(RootComponent);
	DirectionIndicatorPlane->SetRelativeLocation(FVector(20, 0, -70));
	DirectionIndicatorPlane->SetRelativeRotation(FRotator(0, 90, 0));
	DirectionIndicatorPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DirectionIndicatorPlane->SetRelativeScale3D(FVector(3.327123, 3.327123, 1));

	//variables overridden from ACharacter_B
	SpecialMaterialIndex = 0;
	ForceSocketName = "spine2_export_C_jnt";
	PunchesToStun = 4;
	bCanBeHeld = false;
	
	Charge1ThrowStrength = 200000.f;
	Charge2ThrowStrength = 500000.f;
	Charge3ThrowStrength = 1000000.f;
}

void APlayerCharacter_B::BeginPlay()
{
	Super::BeginPlay();

	//Create new material instance and assign it
	if (!DirectionIndicatorPlane)
		return;

	GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	if (!GameInstance) { BError("%s can't find the GameInstance_B! ABORT", *GetNameSafe(this)); return; }

	if (GameInstance->ShouldUseSpreadSheets())
	{
		Table = UDataTable_B::CreateDataTable(FScoreTable::StaticStruct(), "DefaultScoreValues.csv");
		FallScoreAmount = Table->GetRow<FScoreTable>("Fall")->Value;
		FellOutOfWorldScoreAmount = Table->GetRow<FScoreTable>("FellOutOfWorld")->Value;
	}
}

void APlayerCharacter_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetState() == EState::EBeingHeld)
	{
		CurrentHoldTime += DeltaTime;
		if (CurrentHoldTime >= MaxHoldTime)
			BreakFree();
	}

	//	BLog("Last Hit By: %s", *GetNameSafe(LastHitBy));
//	BLog("Last Hit By: %s", *GetNameSafe(LastHitBy));
	//BWarn("Capsule collision profile: %s for player: %s", *GetCapsuleComponent()->GetCollisionProfileName().ToString(), *GetNameSafe(this));

}

void APlayerCharacter_B::FellOutOfWorld(const UDamageType& dmgType)
{
	if (HoldComponent)
		HoldComponent->Drop();
	Die();
	UGameplayStatics::ApplyDamage(this, FellOutOfWorldScoreAmount, PlayerController, this, UOutOfWorld_DamageType_B::StaticClass());
	Super::FellOutOfWorld(dmgType);
}

void APlayerCharacter_B::Die()
{
	Fall();
	bIsAlive = false;
	if (DirectionIndicatorPlane)
		DirectionIndicatorPlane->DestroyComponent();

	PlayerController->TryRespawn(RespawnDelay);
	PlayerController->UnPossess();
}

void APlayerCharacter_B::Fall(FVector MeshForce, float RecoveryTime)
{
	Super::Fall(MeshForce, RecoveryTime);
	bCanBeHeld = true;
	if (PlayerController)
		PlayerController->PlayControllerVibration(1.f, 0.5f, true, true, true, true);
	DirectionIndicatorPlane->SetScalarParameterValueOnMaterials("Health", PunchesToStun);
	
	if (HighShatterSound)
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
	DirectionIndicatorPlane->SetScalarParameterValueOnMaterials("Health", StunAmount);
	MakeInvulnerable(InvulnerabilityTime);
	CurrentHoldTime = 0.f;
}

void APlayerCharacter_B::Dropped_Implementation()
{
	Super::Dropped_Implementation();
	CurrentHoldTime = 0.f;
}

void APlayerCharacter_B::BreakFreeButtonMash()
{
	CurrentHoldTime += 0.01;
}

void APlayerCharacter_B::BreakFree()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorRotation(FRotator(0, 0, 0));
	MakeInvulnerable(1.f);
	//Moves forward a bit before turning on collisions, so we dont fly to hell because we overlap with the other player
	if (HoldingCharacter)
	{
		AddActorLocalOffset(HoldingCharacter->GetActorForwardVector() * 100);
		HoldingCharacter->HoldComponent->SetHoldingItem(nullptr);
		HoldingCharacter->SetState(EState::EWalking);
		HoldingCharacter->AddStun(PunchesToStun-1);
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
	if (DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UOutOfWorld_DamageType_B::StaticClass()))
	{
		if (LastHitBy) // Hit by someone before falling out of the world!
		{
			APlayerController_B* OtherPlayerController = Cast<APlayerController_B>(LastHitBy);
			if (OtherPlayerController)
			{
				OtherPlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddScore(DamageAmount);
			}
		}
		else {
			PlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddScore(-DamageAmount);
		}
	}
	else
	{
		APlayerController_B* OtherPlayerController = Cast<APlayerController_B>(EventInstigator);
		if (OtherPlayerController)
		{
			OtherPlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->AddScore(DamageAmount);
		}
	}

	if (GameInstance)
	{
		GameInstance->PlayImpactCameraShake(GetActorLocation());
		float trauma = DamageAmount / 100;
		if (PlayerController)
			PlayerController->PlayControllerVibration(FMath::Square(trauma), 0.3, true, true, true, true);
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

void APlayerCharacter_B::AddStun(const int Strength)
{
	Super::AddStun(Strength);
	DirectionIndicatorPlane->SetScalarParameterValueOnMaterials("Health", StunAmount);

	float Volume = 1.f;
	GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance)
		Volume *= GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume();

	switch (StunAmount)	//slem gonza	
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

	PlayerController = Cast<APlayerController_B>(NewController);

	if (!(PlayerController && PlayerController->HealthComponent))
		return;

	PlayerInfo.ID = UGameplayStatics::GetPlayerControllerID(PlayerController);

	PlayerController->HealthComponent->HealthIsZero_D.AddUObject(this, &APlayerCharacter_B::Die);
	PlayerController->HealthComponent->RespawnIsZero_D.AddUObject(this, &APlayerCharacter_B::Die);
	if (PlayerController->HealthComponent->HealthWidget)
		PlayerController->HealthComponent->HealthWidget->PostInitialize(this);
	PlayerController->PlayerInfo = PlayerInfo;

	PunchComponent->OnPunchHit_D.AddLambda([&]() //Keeps crashing here after compile -E
		{
			PlayerController->PlayControllerVibration(0.2f, 0.3f, true, true, true, true);
		});

}

//For dashing through characters
void APlayerCharacter_B::OnCapsuleOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Dash push stuff
	Super::OnCapsuleOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,SweepResult);
	
	if (!PunchComponent->GetIsDashing())	
		return;


	ACharacter_B* OtherCharacter = Cast<ACharacter_B>(OtherActor);
	UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(OtherComp);
	//Might be triggered twice
	if (IsValid(OtherCharacter) && IsValid(Capsule))
	{
		if (IsValid(PunchComponent))
			OtherCharacter->GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity * (-PunchComponent->DashPushPercentage);
	}
}
