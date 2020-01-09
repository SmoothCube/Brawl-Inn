// Fill out your copyright notice in the Description page of Project Settings.


#include "DragArea_B.h"
#include "Components/BoxComponent.h"
#include "Player/PlayerCharacter_B.h"
#include "Components/HoldComponent_B.h"
#include "Components/StaticMeshComponent.h"

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

	for (int i=0; i<ActorsToMove.Num(); i++)
	{
		if (ActorsToMove.IsValidIndex(i))
		{
				ActorsToMove[i]->AddActorWorldOffset(GetActorForwardVector() * DragStrength*DeltaTime);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[ADragArea_B::Tick] Invalid Index!: %s"), *GetActorForwardVector().ToString())
		}
	}
}


void ADragArea_B::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bool bShouldMove = true;
	APlayerCharacter_B* Player = Cast<APlayerCharacter_B>(OtherActor);
	if (Player)
	{
		if (OtherComp->IsA(UHoldComponent_B::StaticClass()))
		{
			return;
		}
	}
	ActorsToMove.Add(OtherActor);
	UE_LOG(LogTemp, Warning, TEXT("[ADragArea_B::OnOverlapBegin] Drag Area Overlaps with: %s"), *GetNameSafe(OtherActor));

}

void ADragArea_B::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	ActorsToMove.Remove(OtherActor);
	UE_LOG(LogTemp, Warning, TEXT("[ADragArea_B::OnOverlapEnd] Drag Area Ends Overlaps with: %s"), *GetNameSafe(OtherActor));
}