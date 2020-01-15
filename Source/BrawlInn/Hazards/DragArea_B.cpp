// Fill out your copyright notice in the Description page of Project Settings.


#include "DragArea_B.h"

#include "BrawlInn.h"
#include "Components/BoxComponent.h"
#include "Player/PlayerCharacter_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/HealthComponent_B.h"
#include "Player/PlayerController_B.h"

// Sets default values
ADragArea_B::ADragArea_B()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DragArea = CreateDefaultSubobject<UBoxComponent>("DragArea");
//	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(DragArea);
}

// Called when the game starts or when spawned
void ADragArea_B::BeginPlay()
{
	Super::BeginPlay();
	DragArea->OnComponentBeginOverlap.AddDynamic(this, &ADragArea_B::OnOverlapBegin);
	DragArea->OnComponentEndOverlap.AddDynamic(this, &ADragArea_B::OnOverlapEnd);
}

// Called every frame
void ADragArea_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 0; i < PlayersToMove.Num(); i++)
	{
		if (PlayersToMove.IsValidIndex(i))
		{
			if (PlayersToMove[i]->State == EState::EFallen)
				PlayersToMove[i]->GetMesh()->AddForce(GetActorForwardVector() * DragStrength);
			else
				PlayersToMove[i]->GetCharacterMovement()->AddForce(GetActorForwardVector() * DragStrength);
			PlayersToMove[i]->GetCharacterMovement()->AddInputVector(FVector::ZeroVector);
		}
	}

	for (int i=0; i<ComponentsToMove.Num(); i++)
	{
		if (ComponentsToMove.IsValidIndex(i) && ComponentsToMove[i])
		{
			ComponentsToMove[i]->AddForce(GetActorForwardVector() * DragStrength);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[ADragArea_B::Tick] Invalid Index!: %s"), *GetActorForwardVector().ToString())
		}
	}
}

void ADragArea_B::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter_B* Player = Cast<APlayerCharacter_B>(OtherActor);
	if (Player)
	{
		if (OtherComp->IsA(UHoldComponent_B::StaticClass()))
		{
			return;
		}
		PlayersToMove.Add(Player);
		
		Player->GetPlayerController()->HealthComponent->FireDamageStop_D.Broadcast();
	}
	else
	{
		ComponentsToMove.Add(OtherComp);
	}
}

void ADragArea_B::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	APlayerCharacter_B* Player = Cast<APlayerCharacter_B>(OtherActor);
	if (Player)
	{
		PlayersToMove.Remove(Player);
	}
	ComponentsToMove.Remove(OtherComp);
}