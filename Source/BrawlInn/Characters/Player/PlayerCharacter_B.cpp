// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter_B.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Characters/Player/PlayerController_B.h"
#include "Components/HealthComponent_B.h"
#include "Components/PunchComponent_B.h"
#include "Components/HoldComponent_B.h"
#include "UI/Game/HealthWidget_B.h"
#include "System/DamageTypes/Stool_DamageType_B.h"
#include "System/DamageTypes/Fall_DamageType_B.h"
#include "System/DamageTypes/OutOfWorld_DamageType_B.h"
#include "System/GameInstance_B.h"

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
}

void APlayerCharacter_B::BeginPlay()
{
	Super::BeginPlay();

	//Create new material instance and assign it
	if (!DirectionIndicatorPlane)
		return;

	auto MI_ColoredDecal = UMaterialInstanceDynamic::Create(DirectionIndicatorPlane->GetMaterial(0), this);
	MI_ColoredDecal->SetVectorParameterValue(FName("Color"), PlayerInfo.PlayerColor);
	DirectionIndicatorPlane->SetMaterial(0, MI_ColoredDecal);

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
}

void APlayerCharacter_B::FellOutOfWorld(const UDamageType& dmgType)
{
	if (HoldComponent)
		HoldComponent->Drop();
	UGameplayStatics::ApplyDamage(this, FellOutOfWorldDamageAmount, PlayerController, this, dmgType.StaticClass());
	Super::FellOutOfWorld(dmgType);
}

void APlayerCharacter_B::Die()
{
	Fall(-1);
	bIsAlive = false;
	if (DirectionIndicatorPlane)
		DirectionIndicatorPlane->DestroyComponent();

	PlayerController->TryRespawn(RespawnDelay);
	PlayerController->UnPossess();
}

void APlayerCharacter_B::Fall(float RecoveryTime)
{
	Super::Fall(RecoveryTime);

	if (PlayerController)
		PlayerController->PlayControllerVibration(1.f, 0.5f, true, true, true, true);
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

	//Moves forward a bit before turning on collisions, so we dont fly to hell because we overlap with the other player
	if (HoldingCharacter)
	{
		AddActorLocalOffset(HoldingCharacter->GetActorForwardVector() * 100);
		HoldingCharacter->HoldComponent->SetHoldingItem(nullptr);
		HoldingCharacter->AddStun(PunchesToStun);
		HoldingCharacter = nullptr;
	}

	StandUp();

	CurrentHoldTime = 0.f;
}

void APlayerCharacter_B::RemoveStun()
{
	Super::RemoveStun();
	CurrentHoldTime = 0.f;
}

float APlayerCharacter_B::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

	if (!(DamageAmount == 100.f))
		if (bIsInvulnerable || bHasShield) return 0;

	float ActualDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UFall_DamageType_B::StaticClass()))
	{
		ActualDamageAmount = FallDamageAmount;
	}
	else if (DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UStool_DamageType_B::StaticClass()))
	{
		ActualDamageAmount = ChairDamageAmount;
	}
	else if (DamageEvent.DamageTypeClass.GetDefaultObject()->IsA(UOutOfWorld_DamageType_B::StaticClass()))
	{
		ActualDamageAmount = FellOutOfWorldDamageAmount;
	}
	else
	{
		ActualDamageAmount = DamageAmount;
	}

	UGameInstance_B* GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance)
	{
		GameInstance->PlayImpactCameraShake(GetActorLocation());
		float trauma = DamageAmount / 100;
		if (PlayerController)
			PlayerController->PlayControllerVibration(FMath::Square(trauma), 0.3, true, true, true, true);
	}

	IControllerInterface_B* Interface = Cast<IControllerInterface_B>(GetController());
	if (Interface)
	{
		Interface->Execute_TakeDamage(GetController(), ActualDamageAmount);
	}

	return DamageAmount;
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

	PunchComponent->OnPunchHit_D.AddLambda([&]()
		{
			PlayerController->PlayControllerVibration(0.2f, 0.3f, true, true, true, true);
		});

}
