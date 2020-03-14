// Fill out your copyright notice in the Description page of Project Settings.

#include "BounceActor_B.h"
#include "Components/StaticMeshComponent.h"
#include "DestructibleComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/CapsuleComponent.h"

#include "BrawlInn.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Characters/Player/PlayerController_B.h"
#include "Components/HoldComponent_B.h"
#include "System/DataTable_B.h"
#include "System/GameModes/GameMode_B.h"


ABounceActor_B::ABounceActor_B()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh->SetSimulatePhysics(true);
	PickupCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}

void ABounceActor_B::BeginPlay()
{
	Super::BeginPlay();
	Table = UDataTable_B::CreateDataTable(FScoreTable::StaticStruct(), "DefaultScoreValues.csv");
	ScoreAmount = Table->GetRow<FScoreTable>("Barrel")->Value;

	PickupCapsule->OnComponentBeginOverlap.AddDynamic(this, &ABounceActor_B::OnPickupCapsuleOverlap);
	
}
void ABounceActor_B::OnPickupCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this)
		return;
	
	BLog("HIT");
	BreakBarrel();
}

// Old Explode
void ABounceActor_B::OnItemFracture()
{
	Super::OnItemFracture();

	IThrowableInterface_B* Interface = Cast<IThrowableInterface_B>(this);
	if (Interface && Interface->Execute_IsHeld(this))
	{
		if (OwningCharacter->HoldComponent->GetHoldingItem() == this)
		{
			OwningCharacter->HoldComponent->SetHoldingItem(nullptr);

			OwningCharacter->SetState(EState::EWalking);
		}
	}

	SpawnPlayerCharacter();

	if (PlayerController)
	{
		UGameplayStatics::ApplyRadialDamage(GetWorld(), ScoreAmount, GetActorLocation(), Radius, BP_DamageType, {}, this, PlayerController, true);
	}
}

void ABounceActor_B::SpawnPlayerCharacter()
{

	//Respawns player. Kinda hates having this here.
	if (PlayerController)
	{
		AGameMode_B* GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->SpawnCharacter_D.Broadcast(PlayerController->GetPlayerInfo(), true, FTransform(GetActorLocation()));
		}
	}
	if (PlayerController->GetPlayerCharacter())
		PlayerController->GetPlayerCharacter()->MakeInvulnerable(1.f, true);
}

void ABounceActor_B::SetupBarrel(APlayerController_B* Controller)
{
	PlayerController = Controller;

	SetActorRotation(FRotator(0, 0, 90));

	UMaterialInstanceDynamic* MeshMaterial = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
	MeshMaterial->SetTextureParameterValue("BaseColor", PlayerController->GetPlayerInfo().CharacterVariant.BarrelTexture);

	DestructibleComponent->SetMaterial(0, MeshMaterial);
	DestructibleComponent->SetMaterial(1, MeshMaterial);

	Mesh->SetMaterial(0, MeshMaterial);

}

void ABounceActor_B::BreakBarrel()
{
	DestructibleComponent->ApplyDamage(100, DestructibleComponent->GetComponentLocation(), FVector(0, 0, -1), 1000);
}
