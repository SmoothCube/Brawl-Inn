// Fill out your copyright notice in the Description page of Project Settings.


#include "DragArea_B.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

#include "BrawlInn.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Components/HoldComponent_B.h"
#include "System/GameInstance_B.h"


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

	if (RiverSound)
	{
		float volume = 1.f;
		UGameInstance_B* GameInstance = Cast<UGameInstance_B>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (GameInstance)
		{
			volume *= GameInstance->MasterVolume * GameInstance->SfxVolume;
		}
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			RiverSound,
			GetActorLocation(),
			volume
		);
	}
}

// Called every frame
void ADragArea_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//active players that have not fallen over
	for (int i = 0; i < PlayersToMove.Num(); i++)
	{
		if (PlayersToMove.IsValidIndex(i) && PlayersToMove[i])
		{
			//this is not really how i want it :c
			PlayersToMove[i]->GetCharacterMovement()->AddInputVector(GetActorForwardVector()*InputMultiplier, true);
		}
	}

	//Skeletal meshes
	for (int i = 0; i < SkeletonsToMove.Num(); i++)
	{		
		if (SkeletonsToMove.IsValidIndex(i) && SkeletonsToMove[i])
		{
			SkeletonsToMove[i]->AddForceToAllBodiesBelow(GetActorForwardVector() * SkeletalDragStrength);
		}
		else
		{
			BError("Invalid Index!: %s", *GetActorForwardVector().ToString())
		}
	}
	
	//all other stuff
	for (int i=0; i<ComponentsToMove.Num(); i++)
	{
		if (ComponentsToMove.IsValidIndex(i) && ComponentsToMove[i])
		{
			ComponentsToMove[i]->AddForce(GetActorForwardVector() * ItemDragStrength);
		}
		else
		{
			BError("Invalid Index!: %s", *GetActorForwardVector().ToString());
		}
	}
}

void ADragArea_B::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//only adds to one of the three arrays
	USkeletalMeshComponent* Skeleton = Cast<USkeletalMeshComponent>(OtherComp);
	APlayerCharacter_B* Player = Cast<APlayerCharacter_B>(OtherActor);
	if (Skeleton)
	{
		SkeletonsToMove.Add(Skeleton);
	}
	else if (Player)
	{
		if (OtherComp->IsA(UHoldComponent_B::StaticClass()))
		{
			return;
		}
		PlayersToMove.Add(Player);
		BWarn("Adding player %s", *GetNameSafe(Player));
	}
	else
	{
		ComponentsToMove.Add(OtherComp);
	}
}

void ADragArea_B::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	USkeletalMeshComponent* Skeleton = Cast<USkeletalMeshComponent>(OtherComp);
	APlayerCharacter_B* Player = Cast<APlayerCharacter_B>(OtherActor);
	if (Skeleton)
	{
		SkeletonsToMove.Remove(Skeleton);
	}
	else if (Player)
	{
		PlayersToMove.Remove(Player);

		//Checks to see if the skeletalmesh is still overlapping. Adds it to SkeletonsToMove if it is
		BWarn("Removing player %s", *GetNameSafe(Player));
		TArray<UPrimitiveComponent*> OverlappingComponents;
		GetOverlappingComponents(OverlappingComponents);
		for (auto& comp : OverlappingComponents)
		{
			AActor* Actor = comp->GetOwner();
			APlayerCharacter_B* OtherPlayer = Cast<APlayerCharacter_B>(Actor);
			USkeletalMeshComponent* Mesh = Cast<USkeletalMeshComponent>(comp);
			
			if (OtherPlayer != nullptr && Mesh != nullptr)
			{
				BWarn("Found Mesh %s", *GetNameSafe(Mesh));
				SkeletonsToMove.Add(Mesh);
			}
		}
	}
	else
	{
		ComponentsToMove.Remove(OtherComp);
	}
}