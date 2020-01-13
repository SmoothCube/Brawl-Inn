// Fill out your copyright notice in the Description page of Project Settings.


#include "BounceActor_B.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrawlInn.h"

#include "Player/PlayerCharacter_B.h"
// Sets default values
ABounceActor_B::ABounceActor_B()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
}


// Called when the game starts or when spawned
void ABounceActor_B::BeginPlay()
{
	Super::BeginPlay();
	SetActorRotation(FRotator(0, 50, 0));
	//Mesh->AddForce(FVector(-25000, 25000, 0));

	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ABounceActor_B::OnOverlapBegin);
}

// Called every frame
void ABounceActor_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetActorRotation(FRotator(GetActorRotation().Pitch, 50, GetActorRotation().Roll));

}

void ABounceActor_B::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	BWarn("Barrel Collides with %s", *GetNameSafe(OverlappedComp));
	APlayerCharacter_B* Player = Cast<APlayerCharacter_B>(OtherActor);
	if (Player)
	{
		Player->GetCharacterMovement()->AddImpulse(GetVelocity());
	}
}
