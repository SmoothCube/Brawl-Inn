// Fill out your copyright notice in the Description page of Project Settings.


#include "VictoryGameMode_B.h"

#include "BrawlInn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Camera/CameraActor.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/LocalPlayer.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"

#include "System/GameInstance_B.h"
#include "System/SubSystems/ScoreSubSystem_B.h"

void AVictoryGameMode_B::BeginPlay()
{
	Super::BeginPlay();

	ACameraActor* IntroCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), GameInstance->GetCameraSwapTransform());
	check(IsValid(IntroCamera));
	UpdateViewTargets(IntroCamera);

	BWarn("Scoreboard sorted by most score:");
	for (auto Info : GameInstance->GetPlayerInfos())
	{
		APlayerController* Controller = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), Info.ID);
		BWarn("Player Controller ID: %i with %i as score.", Info.ID, Controller->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().Score);
	}

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Players", OutActors);

	TArray<FName> VictoryTags;
	VictoryTags.Add("First");
	VictoryTags.Add("Second");
	VictoryTags.Add("Third");
	VictoryTags.Add("Fourth");

	//Finds the characters to keep and updates their Mesh Material.
	TArray<AActor*> CharactersToKeep;
	for (int i = 0; i < GameInstance->GetPlayerInfos().Num(); i++)
	{
		const auto Character = OutActors.FindByPredicate([&](AActor* Actor)
			{
				return Actor->ActorHasTag(VictoryTags[i]);
			});
		CharactersToKeep.Add(*Character);
		APlayerCharacter_B* PlayerCharacter = Cast<APlayerCharacter_B>(*Character);
		PlayerCharacter->GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(1, GameInstance->GetPlayerInfos()[i].CharacterVariant.MeshMaterial);
		PlayerCharacter->GetDirectionIndicatorPlane()->DestroyComponent();
	}

	//Removes and destroys the characters that isn't supposed to be there
	for (auto Actor : CharactersToKeep)
		OutActors.Remove(Actor);

	for (auto Actor : OutActors)
		Actor->Destroy();

	checkf(IsValid(FadeToBlackSequence), TEXT("Remember to set FadeToBlackSequence in the blueprint!"));
	ALevelSequenceActor* SequenceActor;
	FMovieSceneSequencePlaybackSettings Settings;
	Settings.bAutoPlay = false;
	Settings.bPauseAtEnd = true;
	FadeToBlackSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), FadeToBlackSequence, Settings, SequenceActor);
	FadeToBlackSequencePlayer->OnFinished.AddDynamic(this, &AVictoryGameMode_B::OnFadeToBlackFinished);
	check(IsValid(FadeToBlackSequencePlayer));

}

void AVictoryGameMode_B::PostLevelLoad()
{
	checkf(IsValid(VictoryCamera_BP), TEXT("VictoryCamera_BP is not set! Make sure to set it in the blueprint!"));

	ACameraActor* VictoryCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), VictoryCamera_BP));

	UpdateViewTargets(VictoryCamera, 3);
}

void AVictoryGameMode_B::StartFadeToScore()
{
	if (bFinalScoreVisible)
		return;

	FadeToBlackSequencePlayer->Play();
	bFinalScoreVisible = true;
}

void AVictoryGameMode_B::OnFadeToBlackFinished()
{
	BScreen("Show UI here");
}