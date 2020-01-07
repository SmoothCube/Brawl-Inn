// Fill out your copyright notice in the Description page of Project Settings.


#include "PunchComponent_B.h"
#include "Player/PlayerCharacter_B.h"
// Sets default values for this component's properties
UPunchComponent_B::UPunchComponent_B()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Player = Cast<APlayerCharacter_B>(GetOwner());
}


// Called when the game starts
void UPunchComponent_B::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UPunchComponent_B::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPunchComponent_B::PunchStart()
{
	UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent_B::PunchStart]"));
}

void UPunchComponent_B::PunchEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("[UPunchComponent_B::PunchEnd]"));
	bIsPunching = false;
}

bool UPunchComponent_B::GetIsPunching()
{
	return bIsPunching;
}