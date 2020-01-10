// Fill out your copyright notice in the Description page of Project Settings.

#include "ThrowComponent_B.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "Player/PlayerCharacter_B.h"
#include "Components/HoldComponent_B.h"
#include "System/GameMode_B.h"

UThrowComponent_B::UThrowComponent_B(const FObjectInitializer& ObjectInitializer)
{
	SphereRadius = 400;
	ShapeColor = FColor::Blue;
	PrimaryComponentTick.bCanEverTick = false;
}

bool UThrowComponent_B::TryThrow()
{
	if (!IsReady())
		return false;

	Throw();
	return true;
}

void UThrowComponent_B::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UThrowComponent_B::BeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UThrowComponent_B::EndOverlap);

	OwningPlayer = Cast<APlayerCharacter_B>(GetOwner());

	GameMode = Cast<AGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->GameModeInit_D.AddDynamic(this, &UThrowComponent_B::GameIsReady);
}

void UThrowComponent_B::GameIsReady()
{
	TArray<AActor*> TempArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter_B::StaticClass(), TempArray);
	for (const auto& Player : TempArray)
		OtherPlayers.Add(Cast<APlayerCharacter_B>(Player));
}

void UThrowComponent_B::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void UThrowComponent_B::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
}

void UThrowComponent_B::Throw()
{
	UE_LOG(LogTemp, Warning, TEXT("Throwing"));
}

bool UThrowComponent_B::IsReady() const
{
	if (GameMode && OwningPlayer)
		return true;
	return false;
}
