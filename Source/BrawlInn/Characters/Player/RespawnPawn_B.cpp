// Fill out your copyright notice in the Description page of Project Settings.

#include "RespawnPawn_B.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "BrawlInn.h"
#include "System/Camera/GameCamera_B.h"
#include "Characters/Player/PlayerController_B.h"
#include "System/GameModes/MainGameMode_B.h"
#include "Hazards/BounceActor/BounceActorSpawner_B.h"
#include "Hazards/BounceActor/BounceActor_B.h"

// Sets default values
ARespawnPawn_B::ARespawnPawn_B()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);

	Decal = CreateDefaultSubobject<UDecalComponent>("Decal");
	Decal->AddLocalRotation(FRotator(90, 0, 0));
	Decal->SetupAttachment(Sphere);
}

// Called when the game starts or when spawned
void ARespawnPawn_B::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(TH_ThrowTimer, this, &ARespawnPawn_B::StartThrow, TimeBeforeCanThrow);

	GameCamera = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()))->GetGameCamera();
	
	Cannon = Cast<ABounceActorSpawner_B>(UGameplayStatics::GetActorOfClass(GetWorld(), ABounceActorSpawner_B::StaticClass()));
	if (Cannon)
		Cannon->AddRotateTarget(this);
	else
		BError("No BarrelSpawner Found for RespawnPawn %s", *GetNameSafe(this));
	check(GameCamera != nullptr);

}

void ARespawnPawn_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void ARespawnPawn_B::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//set the color for the decal
	if (Decal)
	{
		auto MI_ColoredDecal = UMaterialInstanceDynamic::Create(Decal->GetMaterial(0), this);
		APlayerController_B* PlayerController = Cast<APlayerController_B>(NewController);
		if (PlayerController)
			MI_ColoredDecal->SetVectorParameterValue(FName("Color"), PlayerController->GetPlayerInfo().CharacterVariant.TextColor);
		else
			BWarn("No player controller found for RespawnPawn %s", *GetNameSafe(this));
		Decal->SetMaterial(0, MI_ColoredDecal);
	}
}

void ARespawnPawn_B::StartThrow()
{
	GetWorld()->GetTimerManager().SetTimer(TH_ThrowTimer, this, &ARespawnPawn_B::ThrowBarrel, TimeUntilAutoThrow);
	bThrowHasStarted = true;
}

void ARespawnPawn_B::ThrowBarrel()
{
	if (!bThrowHasStarted)
		return;
	
	if (!bBarrelIsThrown)
	{
		check(IsValid(Cannon));

		Cannon->AddShootTarget(this);

		Cannon->RemoveRotateTarget(this);
		//Barrel now spawns the player...
		bBarrelIsThrown = true;

	}
	else if (bCanBreakBarrel && Barrel)
	{
		Barrel->BreakBarrel();
	}
}

void ARespawnPawn_B::SetupBarrel(ABounceActor_B* NewBarrel)
{
	if (NewBarrel)
	{
		Barrel = NewBarrel;
		NewBarrel->SetupBarrel(Cast<APlayerController_B>(GetController()));
		AMainGameMode_B* GameMode = Cast<AMainGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->AddCameraFocusPoint(NewBarrel);
			GameMode->RemoveCameraFocusPoint(this);
		}
		GetWorld()->GetTimerManager().SetTimer(TH_ThrowTimer, this, &ARespawnPawn_B::SetCanBreakBarrel, TimeBeforeCanBreakBarrel);
	}
}

void ARespawnPawn_B::SetCanBreakBarrel()
{
	bCanBreakBarrel = true;
}

void ARespawnPawn_B::SetInputVectorX(const float X)
{
	if (!bBarrelIsThrown)
	{
		InputVector.X = X;
		if (InputVector.SizeSquared() >= 1.f)
			InputVector.Normalize();
		AddActorLocalOffset(GameCamera->GetActorForwardVector() * InputVector.X * GetWorld()->GetDeltaSeconds() * MovementSpeed, true);
	}
}

void ARespawnPawn_B::SetInputVectorY(const float Y)
{
	if (!bBarrelIsThrown)
	{
		InputVector.Y = Y;
		if (InputVector.SizeSquared() >= 1.f)
			InputVector.Normalize();
		AddActorWorldOffset(GameCamera->GetActorRightVector() * InputVector.Y * GetWorld()->GetDeltaSeconds() * MovementSpeed, true);
	}
}

