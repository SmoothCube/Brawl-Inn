// Fill out your copyright notice in the Description page of Project Settings.


#include "DragArea_B.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include "BrawlInn.h"
#include "Characters/Character_B.h"
#include "Components/HoldComponent_B.h"
#include "System/GameInstance_B.h"


// Sets default values
ADragArea_B::ADragArea_B()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DragArea = CreateDefaultSubobject<UBoxComponent>("DragArea");
	RiverSoundComponent = CreateDefaultSubobject<UAudioComponent>("RiverSoundComponent");
//	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(DragArea);
}

// Called when the game starts or when spawned
void ADragArea_B::BeginPlay()
{
	Super::BeginPlay();
	DragArea->OnComponentBeginOverlap.AddDynamic(this, &ADragArea_B::OnOverlapBegin);
	DragArea->OnComponentEndOverlap.AddDynamic(this, &ADragArea_B::OnOverlapEnd);
	if (RiverSoundComponent)
	{
		UGameInstance_B* GameInstance = Cast<UGameInstance_B>(GetGameInstance());
		if (GameInstance)
			RiverSoundComponent->SetVolumeMultiplier(GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume());
		RiverSoundComponent->Play(FMath::FRandRange(0, 100));
	}

}

// Called every frame
void ADragArea_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//active players that have not fallen over
	TArray<ACharacter_B*> ActorsToRemove;
	for (int i = 0; i < PlayersToMove.Num(); i++)
	{
		if (PlayersToMove.IsValidIndex(i) && PlayersToMove[i])
		{
			//this is not really how i want it :c
			PlayersToMove[i]->GetCharacterMovement()->AddInputVector(GetActorForwardVector()*InputMultiplier, true);
		}
		else
		{
			ActorsToRemove.Add(PlayersToMove[i]);
		}
	}
	for (auto& a : ActorsToRemove)
	{
		PlayersToMove.Remove(a);
	}


	//Skeletal meshes
	TArray<USkeletalMeshComponent*> SkeletonsToRemove;
	for (int i = 0; i < SkeletonsToMove.Num(); i++)
	{		
		if (SkeletonsToMove.IsValidIndex(i) && SkeletonsToMove[i])
		{
			SkeletonsToMove[i]->AddForceToAllBodiesBelow(GetActorForwardVector() * SkeletalDragStrength);
		}
		else
		{
			BError("Invalid SkeletalMesh in River! Index: %d", i);
			SkeletonsToRemove.Add(SkeletonsToMove[i]);
		}
	}

	for (auto& a : SkeletonsToRemove)
	{
		SkeletonsToMove.Remove(a);
	}

	TArray<UPrimitiveComponent*> ComponentsToRemove;

	//all other stuff
	for (int i=0; i<ComponentsToMove.Num(); i++)
	{
		if (ComponentsToMove.IsValidIndex(i) && ComponentsToMove[i])
		{
			ComponentsToMove[i]->AddForce(GetActorForwardVector() * ItemDragStrength);
		}
		else
		{
			BError("Invalid Component in river! Index: %s", *GetActorForwardVector().ToString());
			ComponentsToRemove.Add(ComponentsToMove[i]);
		}
	}

	for (auto& a : ComponentsToRemove)
	{
		ComponentsToMove.Remove(a);
	}
}

void ADragArea_B::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	//only adds to one of the three arrays
	USkeletalMeshComponent* Skeleton = Cast<USkeletalMeshComponent>(OtherComp);
	ACharacter_B* Character = Cast<ACharacter_B>(OtherActor);
	if (Skeleton && SkeletonsToMove.Find(Skeleton) == INDEX_NONE)
	{
		SkeletonsToMove.Add(Skeleton);
	}
	else if (Character)
	{
		if (OtherComp->IsA(UHoldComponent_B::StaticClass()))
		{
			return;
		}
		USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
		if(IsValid(CharacterMesh) && SkeletonsToMove.Find(CharacterMesh) == INDEX_NONE)
			SkeletonsToMove.Add(Character->GetMesh());
	}
	else
	{
		ComponentsToMove.Add(OtherComp);
	}
}

void ADragArea_B::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	USkeletalMeshComponent* Skeleton = Cast<USkeletalMeshComponent>(OtherComp);
	ACharacter_B* Player = Cast<ACharacter_B>(OtherActor);
	if (Skeleton)
	{
		SkeletonsToMove.Remove(Skeleton);
	}
	else if (Player)
	{
		PlayersToMove.Remove(Player);

		//Checks to see if the skeletalmesh is still overlapping. Adds it to SkeletonsToMove if it is
		TArray<UPrimitiveComponent*> OverlappingComponents;
		GetOverlappingComponents(OverlappingComponents);
		for (auto& comp : OverlappingComponents)
		{
			AActor* Actor = comp->GetOwner();
			ACharacter_B* OtherPlayer = Cast<ACharacter_B>(Actor);
			USkeletalMeshComponent* Mesh = Cast<USkeletalMeshComponent>(comp);
			
			if (OtherPlayer != nullptr && Mesh != nullptr)
			{
				SkeletonsToMove.Add(Mesh);
			}
		}
	}
	else
	{
		ComponentsToMove.Remove(OtherComp);
	}
}