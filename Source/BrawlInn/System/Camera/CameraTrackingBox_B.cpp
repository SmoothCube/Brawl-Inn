// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraTrackingBox_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "BrawlInn.h"

#include "Player/PlayerController_B.h"
#include "Player/PlayerCharacter_B.h"

ACameraTrackingBox_B::ACameraTrackingBox_B()
{

}

void ACameraTrackingBox_B::BeginPlay()
{
	
	
	TArray<AActor*> TempPlayerControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController_B::StaticClass(), TempPlayerControllers);
	for (const auto& Controller : TempPlayerControllers)
	{
		PlayerControllers.Add(Cast<APlayerController_B>(Controller));
	}
	
	/*TArray<AActor*> Characters;
	GetOverlappingActors(Characters, APlayerCharacter_B::StaticClass());

	for (const auto& character : Characters)
	{
		APlayerCharacter_B* Player = Cast<APlayerCharacter_B>(character);
		if(Player && Player->GetPlayerController_B())
			FocusPoints.Add(Cast<USceneComponent>(Player->GetMesh()));
	}*/

	OnActorBeginOverlap.AddDynamic(this, &ACameraTrackingBox_B::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ACameraTrackingBox_B::OnEndOverlap);
}

void ACameraTrackingBox_B::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{

	APlayerCharacter_B* Character = Cast<APlayerCharacter_B>(OtherActor);
	if (Character)
	{
		if(Character->GetPlayerController_B())
			PlayerControllers.Add(Character->GetPlayerController_B());
		else
			BWarn("Cant Find Player Controller");
	}
}

void ACameraTrackingBox_B::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{

	APlayerCharacter_B* Character = Cast<APlayerCharacter_B>(OtherActor);
	if (Character && Character->GetPlayerController_B())
		PlayerControllers.Remove(Character->GetPlayerController_B());
}