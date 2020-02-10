// Fill out your copyright notice in the Description page of Project Settings.

#include "Bar_B.h"
#include "Sound/SoundCue.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

#include "BrawlInn.h"
#include "Components/BarMeshComponent_B.h"
#include "Characters/AI/AIController_B.h"
#include "Items/Useable_B.h"
#include "Items/Item_B.h"
#include "System/GameInstance_B.h"

ABar_B::ABar_B()
{
	PrimaryActorTick.bCanEverTick = false;

	House = CreateDefaultSubobject<UBarMeshComponent_B>("House");
	SetRootComponent(House);

	Door = CreateDefaultSubobject<UStaticMeshComponent>("Door");
	Door->SetupAttachment(House);

	ItemSpawnLocation = CreateDefaultSubobject<USceneComponent>("Item spawnlocation");
	ItemSpawnLocation->SetupAttachment(House);

}

void ABar_B::BeginPlay()
{
	Super::BeginPlay();

	StartTimerForNextTankard();
	StartTimerForNextStool();

	AIController = Cast<AAIController_B>(UGameplayStatics::GetActorOfClass(GetWorld(), AAIController_B::StaticClass()));
	if (AIController)
	{
		AIController->OnAIArrivedHome_D.AddUObject(this, &ABar_B::StartTimerForNextStool);
	}
	else
	{
		BError("AI Controller not found!");
	}
	House->OnItemDetach.AddUObject(this, &ABar_B::StartTimerForNextTankard);
}

void ABar_B::SpawnTankard()
{
	if (BP_Useables.Num() == 0)
		return;

	int RandomIndex = FMath::RandRange(0, BP_Useables.Num() - 1);
	AUseable_B* Item = GetWorld()->SpawnActor<AUseable_B>(BP_Useables[RandomIndex], House->GetSocketTransform(ItemSocket));
	Item->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform, ItemSocket);

	if (TankardSpawnSound)
	{
		float volume = 1.f;
		UGameInstance_B* GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (GameInstance)
		{
			volume = GameInstance->MasterVolume * GameInstance->SfxVolume;
		}
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			TankardSpawnSound,
			House->GetSocketLocation(ItemSocket),
			volume
		);
	}
	if (TankardSpawnParticle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TankardSpawnParticle, House->GetSocketLocation(ItemSocket), FRotator(90, 0, 0),FVector(5,5,5));

	}
}

void ABar_B::StartTimerForNextTankard()
{
	GetWorld()->GetTimerManager().SetTimer(TH_NextTankardTimer, this, &ABar_B::SpawnTankard, FMath::FRandRange(MinTankardSpawnTimer, MaxTankardSpawnTimer), false);
}

void ABar_B::SpawnStool()
{
	StoolToDeliver = GetWorld()->SpawnActor<AItem_B>(BP_Stool, ItemSpawnLocation->GetComponentTransform());
	if (IsValid(StoolToDeliver))
	{
		if (AIController)
		{
			AIController->OnStoolReceived_D.Broadcast(StoolToDeliver);
		}
	}

}

void ABar_B::StartTimerForNextStool()
{
	GetWorld()->GetTimerManager().SetTimer(TH_NextStoolTimer, this, &ABar_B::SpawnStool, FMath::FRandRange(MinStoolSpawnTimer, MaxStoolSpawnTimer), false);
}
