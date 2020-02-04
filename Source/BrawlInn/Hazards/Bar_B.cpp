// Fill out your copyright notice in the Description page of Project Settings.

#include "Bar_B.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

#include "BrawlInn.h"
#include "Components/BarMeshComponent_B.h"
#include "Items/Useable_B.h"
#include "System/GameInstance_B.h"

ABar_B::ABar_B()
{
	PrimaryActorTick.bCanEverTick = false;

	House = CreateDefaultSubobject<UBarMeshComponent_B>("House");
	SetRootComponent(House);
}

void ABar_B::BeginPlay()
{
	Super::BeginPlay();
	
	StartTimerForNextSpawn();
	
	House->OnItemDetach.AddUObject(this, &ABar_B::StartTimerForNextSpawn);
}

void ABar_B::SpawnUseable()
{
	if (BP_Useables.Num() == 0)
		return;

	int RandomIndex = FMath::RandRange(0, BP_Useables.Num() - 1);
	AUseable_B* Item = GetWorld()->SpawnActor<AUseable_B>(BP_Useables[RandomIndex], House->GetSocketTransform(ItemSocket));
	Item->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform, ItemSocket);


	if (ItemSpawnSound)
	{
		float volume = 1.f;
		UGameInstance_B* GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (GameInstance)
		{
			volume = GameInstance->MasterVolume * GameInstance->MusicVolume;
		}
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ItemSpawnSound,
			House->GetSocketLocation(ItemSocket),
			volume
			);
	}
}

void ABar_B::StartTimerForNextSpawn()
{
	GetWorld()->GetTimerManager().SetTimer(TH_NextSpawnTimer, this, &ABar_B::SpawnUseable, FMath::FRandRange(MinSpawnTimer, MaxSpawnTimer), false);
}
