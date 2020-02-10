// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter_B.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "BrawlInn.h"
#include "System/GameModes/GameMode_B.h"
#include "Characters/Player/PlayerController_B.h"
#include "Components/HealthComponent_B.h"
#include "UI/Game/HealthWidget_B.h"
APlayerCharacter_B::APlayerCharacter_B()
{
	DirectionIndicatorPlane = CreateDefaultSubobject<UStaticMeshComponent>("Direction Indicator Plane");
	DirectionIndicatorPlane->SetupAttachment(RootComponent);
	DirectionIndicatorPlane->SetRelativeLocation(FVector(20, 0, -70));
	DirectionIndicatorPlane->SetRelativeRotation(FRotator(0, 90, 0));
	DirectionIndicatorPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DirectionIndicatorPlane->SetRelativeScale3D(FVector(3.327123, 3.327123, 1));
}

void APlayerCharacter_B::BeginPlay()
{
	Super::BeginPlay();

	BLog("Type: %i", Type);

	
}

void APlayerCharacter_B::Die()
{
	Fall(-1);
	//PlayerController->UnPossess();
	PlayerController->Player = nullptr;
	DirectionIndicatorPlane->SetHiddenInGame(true);

	//GetWorld()->GetTimerManager().SetTimer(TH_RespawnTimer, this, &APlayerCharacter_B::StartRespawn, RespawnDelay, false);
	//StartRespawn();
}

void APlayerCharacter_B::StartRespawn()
{
	AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		FPlayerInfo Info;
		Info.ID = PlayerController->GetLocalPlayer()->GetControllerId();
		Info.Type = Type;
		if (PlayerController->HealthComponent && PlayerController->HealthComponent->GetRespawns() <= 0)
		{
			PlayerController->HealthComponent->RespawnIsZero_D.Broadcast();
			GameMode->DespawnCharacter_D.Broadcast(Info, false);
		}
		else
		{
			GameMode->DespawnCharacter_D.Broadcast(Info, true);
		}
	}
}

void APlayerCharacter_B::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PlayerController = Cast<APlayerController_B>(NewController);

	if (PlayerController && PlayerController->HealthComponent)
	{
		PlayerController->HealthComponent->HealthIsZero_D.AddUObject(this, &APlayerCharacter_B::Die);
		PlayerController->CharacterType = Type;
	}
	BScreen("LocalPlayer: %i", PlayerController->GetLocalPlayer()->GetControllerId());
}

void APlayerCharacter_B::FellOutOfWorld(const UDamageType& dmgType)
{
	Super::FellOutOfWorld(dmgType);
	UGameplayStatics::ApplyDamage(this, FellOutOfWorldDamageAmount, PlayerController, this, dmgType.StaticClass());
	if (PlayerController)
	{
		FPlayerInfo Info;
		Info.ID = UGameplayStatics::GetPlayerControllerID(PlayerController);
		Info.Type = Type;

		AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			if (PlayerController)
			{
				if (PlayerController->HealthComponent->GetRespawns() <= 0)
					GameMode->DespawnCharacter_D.Broadcast(Info, false);
				else
					GameMode->DespawnCharacter_D.Broadcast(Info, true);
			}
			else
				GameMode->DespawnCharacter_D.Broadcast(Info, true);
		}
		else
		{
			BError("GameMode Could Not Be Found!");
		}
		Destroy();
	}
	else
	{
		Destroy();
	}
}