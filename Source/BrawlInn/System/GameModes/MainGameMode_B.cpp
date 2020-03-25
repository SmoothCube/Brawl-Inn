// Fill out your copyright notice in the Description page of Project Settings.

#include "MainGameMode_B.h"
#include "Engine/World.h"
#include "Engine/TriggerBox.h"
#include "Components/ShapeComponent.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Math/UnrealMathUtility.h"
#include "TimerManager.h"

#include "BrawlInn.h"
#include "Characters/Player/GamePlayerController_B.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "System/Camera/GameCamera_B.h"
#include "System/GameInstance_B.h"
#include "System/SubSystems/ScoreSubSystem_B.h"
#include "System/Structs/ScoreLookupTable.h"
#include "System/DataTable_B.h"
#include "UI/Menus/PauseMenu_B.h"
#include "UI/Game/VictoryScreenWidget_B.h"
#include "UI/Game/GameOverlay_B.h"
#include "Items/LeaderFollower_B.h"

AMainGameMode_B::AMainGameMode_B()
{
	MainMusicComponent = CreateDefaultSubobject<UAudioComponent>("MainMusicComponent");
	SetRootComponent(MainMusicComponent);
	if (!MainMusicComponent)
		BError("MusicComponet Not created properly!");
}

void AMainGameMode_B::BeginPlay()
{
	Super::BeginPlay();

	MainMusicComponent->Stop();

	//find and cache score values
	GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	if (!GameInstance) { BError("%s can't find the GameInstance_B! ABORT", *GetNameSafe(this)); return; }

	if (GameInstance->ShouldUseSpreadSheets())
	{
		UDataTable_B* Table = NewObject<UDataTable_B>();
		Table->LoadCSVFile(FScoreTable::StaticStruct(), "DefaultScoreValues.csv");
		AgainstLeaderMultiplier = Table->GetRow<FScoreTable>("AgainstLeaderMultiplier")->Value;
	}
	/// Spawns and setups cameras
	GameCamera = GetWorld()->SpawnActor<AGameCamera_B>(BP_GameCamera, FTransform());
	FromCharacterSelectionCamera = GetWorld()->SpawnActor<ACameraActor>(BP_FromCharacterSelectionCamera, GameInstance->GetCameraSwapTransform());

	/// Create overlay
	Overlay = CreateWidget<UGameOverlay_B>(GetWorld(), BP_GameOverlay);
	Overlay->UpdateTimerText(TimeRemaining);
	Overlay->AddToViewport();

	/// Spawns characters for the players
	for (const FPlayerInfo Info : GameInstance->GetPlayerInfos())
	{
		AGamePlayerController_B* PlayerController = Cast<AGamePlayerController_B>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), Info.ID));
		if (!PlayerController) { BError("PlayerController for id %i not found. Check IDs in GameInstance", Info.ID); continue; }

		PlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->OnScoreValuesChanged_NoParam.AddUObject(this, &AMainGameMode_B::CallOnAnyScoreChange);

		SpawnCharacter_D.Broadcast(Info, false, FTransform());

	}
	OnGameOver.AddUObject(this, &AMainGameMode_B::EndGame);

	///Countdown
	if (!GameInstance->IgnoreCountdown())
	{
		DisableControllerInputs();

		UpdateViewTargets(FromCharacterSelectionCamera);

		GetWorld()->GetTimerManager().SetTimer(SwapCameraHandle, [&]()
			{
				UpdateViewTargets(nullptr, 2, true);
				GetWorld()->GetTimerManager().SetTimer(StartGameHandle, [&]()
					{
						PreStartGame();
					}, 0.1f, false, 2);
			}, 0.1f, false, 1);
	}
	else
	{
		StartGame();
	}

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ATriggerBox::StaticClass(), "Camera", Actors);
	TrackingBox = Cast<ATriggerBox>(Actors[0]);
	if (TrackingBox)
		TrackingBox->GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &AMainGameMode_B::OnTrackingBoxEndOverlap);

	
}

void AMainGameMode_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AMainGameMode_B::PreStartGame()
{
	if (GameInstance && Countdown)
		UGameplayStatics::PlaySound2D(GetWorld(), Countdown, 0.75 * GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume(), 1.0f);

	GetWorld()->GetTimerManager().SetTimer(CountDownHandle, [&]()
	{
		if (Overlay)
			Overlay->UpdateTimerText(Timer--);
		
	}, 1, true, 0);

	GetWorld()->GetTimerManager().SetTimer(StartGameHandle2, [&]()
		{
			GetWorld()->GetTimerManager().ClearTimer(CountDownHandle);
			StartGame();
		}, 0.1f, false, Countdown->GetDuration());
}

void AMainGameMode_B::UpdateClock()
{
	if (!Overlay)
		return;
	
	Overlay->UpdateTimerText(--TimeRemaining);
	if (TimeRemaining < 0)
		OnGameOver.Broadcast();
}

void AMainGameMode_B::StartGame()
{
	OnGameStart.Broadcast();

	if (GameInstance->GameIsScoreBased())
		GetWorld()->GetTimerManager().SetTimer(TH_CountdownTimer, this, &AMainGameMode_B::UpdateClock, 1.f, true);

	GetWorld()->GetTimerManager().SetTimer(StartGameHandle, this, &AMainGameMode_B::StartMultiplyingScores, TimeBeforeMultiplyScoreAgainstLeader, false);

	EnableControllerInputs();

	if (GameInstance && Birds)
		UGameplayStatics::PlaySound2D(GetWorld(), Birds,
			0.75 * GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume(), 1.0f,
			FMath::FRandRange(0, 100));

	if (GameInstance && MainMusicComponent)
	{
		MainMusicComponent->SetSound(Music);
		MainMusicComponent->SetVolumeMultiplier(GameInstance->GetMasterVolume() * GameInstance->GetMusicVolume());
		MainMusicComponent->Play();
	}

}

void AMainGameMode_B::EndGame()
{
	GetWorld()->GetTimerManager().PauseTimer(TH_CountdownTimer);
	auto LeadingControllers = GetLeadingPlayerController();
	BWarn("Number of leaders: %d", LeadingControllers.Num());

	if (LeadingControllers.IsValidIndex(0) && LeadingControllers[0])
	{
		UVictoryScreenWidget_B* VictoryScreen = CreateWidget<UVictoryScreenWidget_B>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), UGameplayStatics::GetPlayerControllerID(LeadingControllers[0])), BP_VictoryScreen);
		VictoryScreen->AddToViewport();
	}
}
void AMainGameMode_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(PauseMenuWidget))
		return;
	
	PauseMenuWidget->MenuTick();
}

void AMainGameMode_B::PauseGame(AGamePlayerController_B* ControllerThatPaused)
{
	PlayerControllerThatPaused = ControllerThatPaused;
	PauseMenuWidget = CreateWidget<UPauseMenu_B>(ControllerThatPaused, BP_PauseMenu);
	PauseMenuWidget->AddToViewport();
	const FInputModeUIOnly InputMode;
	ControllerThatPaused->SetInputMode(InputMode);
	SetTickableWhenPaused(true);
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void AMainGameMode_B::ResumeGame()
{
	PauseMenuWidget->RemoveFromViewport();
	if (PlayerControllerThatPaused)
	{
		const FInputModeGameOnly InputMode;
		PlayerControllerThatPaused->SetInputMode(InputMode);
	}
	SetTickableWhenPaused(true);
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void AMainGameMode_B::CallOnAnyScoreChange() const
{
	OnAnyScoreChange.Broadcast();
}

void AMainGameMode_B::OnTrackingBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	ACharacter_B* Character = Cast<ACharacter_B>(OtherActor);
	if (Character)
	{
		//Checks to see if the player is still overlapping. Same method as in DragArea
		TArray<AActor*> OverlappingActors;
		if (TrackingBox)
			TrackingBox->GetOverlappingActors(OverlappingActors);
		if (OverlappingActors.Find(Character) == INDEX_NONE)
		{
			if (!(Character->GetState() == EState::EBeingHeld) && Character->IsAlive())	//this is an ugly fix. When a player is picked up, this is run and causes a lot of bugs otherwise.
				Character->Die();
		}
	}
}

void AMainGameMode_B::SetMusic(USoundCue* NewMusic)
{
	if (NewMusic && MainMusicComponent)
		MainMusicComponent->SetSound(NewMusic);
	else
		BWarn("New Music invalid!");
}

void AMainGameMode_B::ResetMusic()
{
	if (MainMusicComponent && (MainMusicComponent->Sound != Music))
	{
		MainMusicComponent->SetSound(Music);
		MainMusicComponent->SetVolumeMultiplier(GameInstance->GetMasterVolume() * GameInstance->GetMusicVolume());
		MainMusicComponent->Play();
	}
}

TArray<AGamePlayerController_B*> AMainGameMode_B::GetLeadingPlayerController()
{
	TArray<AGamePlayerController_B*> TempPlayerControllers = PlayerControllers;
	TempPlayerControllers.Sort([&](const AGamePlayerController_B& Left, const AGamePlayerController_B& Right)
	{
		return Left.GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().Score > Right.GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().Score;
	});
	if (!TempPlayerControllers.IsValidIndex(0) || !TempPlayerControllers[0])
	{
		BError("Invalid Leader!");  return {};
	}

	TArray<AGamePlayerController_B*> ControllersToRemove;
	int HighestScore = TempPlayerControllers[0]->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().Score;
	for (int i = 1; i < TempPlayerControllers.Num(); i++)
	{
		if (TempPlayerControllers[i]->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().Score != HighestScore)
		{
			ControllersToRemove.Add(TempPlayerControllers[i]);
		}
	}
	for (auto c : ControllersToRemove)
	{
		TempPlayerControllers.Remove(c);
	}

	return TempPlayerControllers;
}

void AMainGameMode_B::StartMultiplyingScores()
{
	bMultiplyScoresAgainstLeader = true;

	if (!GetWorld()->SpawnActor<ALeaderFollower_B>(BP_LeaderFollower, FTransform()))
		BError("Spawning LeaderFollower Failed!");
}

bool AMainGameMode_B::ShouldUseScoreMultiplier() const
{
	return bMultiplyScoresAgainstLeader;
}
