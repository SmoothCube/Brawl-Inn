// Fill out your copyright notice in the Description page of Project Settings.

#include "MainGameMode_B.h"
#include "Camera/CameraActor.h"
#include "Components/AudioComponent.h"
#include "Components/ShapeComponent.h"
#include "Engine/TriggerBox.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"

#include "BrawlInn.h"
#include "Characters/Player/GamePlayerController_B.h"
#include "Characters/Player/PlayerCharacter_B.h"
#include "Items/LeaderFollower_B.h"
#include "System/Camera/GameCamera_B.h"
#include "System/DataTable_B.h"
#include "System/Utils.h"
#include "System/GameInstance_B.h"
#include "System/Structs/ScoreLookupTable.h"
#include "System/SubSystems/ScoreSubSystem_B.h"
#include "UI/Widgets/GameOverlay_B.h"
#include "Hazards/Bar_B.h"
#include "UI/Widgets/PauseMenu_B.h"
#include "UI/Widgets/VictoryScreenWidget_B.h"

AMainGameMode_B::AMainGameMode_B()
{
	BarComponent = CreateDefaultSubobject<UBar_B>("BarComponent");
}

void AMainGameMode_B::BeginPlay()
{
	Super::BeginPlay();
}

void AMainGameMode_B::PostLevelLoad()
{
	Super::PostLevelLoad();
	//find and cache score values
	GameInstance = Cast<UGameInstance_B>(GetGameInstance());
	checkf(IsValid(GameInstance), TEXT("%s can't find the GameInstance_B! ABORT"), *GetNameSafe(this));

	if (GameInstance->ShouldUseSpreadSheets())
	{
		UDataTable_B* Table = NewObject<UDataTable_B>();
		Table->LoadCSVFile(FScoreTable::StaticStruct(), "DefaultScoreValues.csv");
		AgainstLeaderMultiplier = Table->GetRow<FScoreTable>("AgainstLeaderMultiplier")->Value;
		Table->ConditionalBeginDestroy();
	}
	/// Spawns and setups cameras
	GameCamera = GetWorld()->SpawnActor<AGameCamera_B>(BP_GameCamera, FTransform());
	FromCharacterSelectionCamera = GetWorld()->SpawnActor<ACameraActor>(BP_FromCharacterSelectionCamera, GameInstance->GetCameraSwapTransform());

	/// Create overlay
	Overlay = CreateWidget<UGameOverlay_B>(GetWorld(), BP_GameOverlay);
	Overlay->UpdateTimerText(GameTimeRemaining);
	Overlay->AddToViewport();

	/// Spawns characters for the players
	for (const FPlayerInfo Info : GameInstance->GetPlayerInfos())
	{
		AGamePlayerController_B* PlayerController = Cast<AGamePlayerController_B>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), Info.ID));
		if (!PlayerController) { BError("PlayerController for id %i not found. Check IDs in GameInstance", Info.ID); continue; }

		PlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->ResetScoreValues();
		PlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->OnScoreChangedNoParam.AddUObject(this, &AMainGameMode_B::CallOnAnyScoreChange);

		SpawnCharacter_D.Broadcast(Info, false, FTransform());
	}

	UpdateViewTargets();


	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ATriggerBox::StaticClass(), "Camera", Actors);
	TrackingBox = Cast<ATriggerBox>(Actors[0]);
	if (TrackingBox)
		TrackingBox->GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &AMainGameMode_B::OnTrackingBoxEndOverlap);

	// Game is ready to start! Starting!
	if (!GameInstance->IgnoreCountdown())
		PreGame();
	else
		StartGame();
}


void AMainGameMode_B::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AMainGameMode_B::PreGame()
{
	DisableControllerInputs();

	UpdateViewTargets(FromCharacterSelectionCamera);

	//Swap camera after 1 second
	BI::Delay(this, 1, [&]()
		{
			UpdateViewTargets(nullptr, 1, true);

			//Start countdown after 1 second this because it takes one second for the PregameCountdownClock begins.
			BI::Delay(this, 1, [&]() { PregameCountdown(); });

			BI::Repeat(this, 1, 4, [&]() { PregameCountdownClock(); });
		});
}

void AMainGameMode_B::PregameCountdown()
{
	check(IsValid(GameInstance) && IsValid(Countdown));

	UGameplayStatics::PlaySound2D(GetWorld(), Countdown, 0.75 * GameInstance->GetMasterVolume() * GameInstance->GetSfxVolume(), 1.0f);

	// Start game when sound is finished
	BI::Delay(this, Countdown->GetDuration(), [&]() {StartGame(); });
}

void AMainGameMode_B::PregameCountdownClock()
{
	check(IsValid(Overlay));

	Overlay->UpdateTimerText(PreGameCountdownTimer--);
}

void AMainGameMode_B::UpdateClock()
{
	if (!Overlay)
		return;

	Overlay->UpdateTimerText(--GameTimeRemaining);
	if (GameTimeRemaining < 0)
		EndGame();
}

void AMainGameMode_B::StartGame()
{
	OnGameStart.Broadcast();

	GetWorld()->GetTimerManager().SetTimer(TH_CountdownTimer, this, &AMainGameMode_B::UpdateClock, 1.f, true);

	GetWorld()->GetTimerManager().SetTimer(StartGameHandle, this, &AMainGameMode_B::StartMultiplyingScores, TimeBeforeMultiplyScoreAgainstLeader, false);

	EnableControllerInputs();

	if (GameInstance)
		GameInstance->SetAndPlayMusic(Music);
}

void AMainGameMode_B::EndGame()
{
	bGameIsOver = true;
	GetWorld()->GetTimerManager().PauseTimer(TH_CountdownTimer);
	//TODO EndGame needs to be implemented.

	DisableControllerInputs();
	LeaderFollower->Destroy();
	

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACameraActor::StaticClass(), "EndGame", OutActors);
	check(OutActors.Num() != 0);
	ACameraActor* OutroCamera = Cast<ACameraActor>(OutActors[0]);

	GameCamera->SetActorTickEnabled(false);
	GameInstance->SetCameraSwapTransform(OutroCamera->GetActorTransform());

	const float BlendTime = 3.f;

	UpdateViewTargets(OutroCamera, BlendTime, true);
	BI::Delay(this, BlendTime, [&]() {UGameplayStatics::OpenLevel(GetWorld(), *GameInstance->GetVictoryMapName()); });

	auto TempPlayerControllers = PlayerControllers;
	SortPlayerControllersByScore(TempPlayerControllers);
	
	TArray<FPlayerInfo> PlayerInfos = GameInstance->GetPlayerInfos();
	PlayerInfos.Sort([&](const FPlayerInfo Left, const FPlayerInfo Right)
		{
			APlayerController* LeftController = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), Left.ID);
			APlayerController* RightController = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), Right.ID);

			 int LeftScore = LeftController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().Score;
			 int RightScore = RightController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().Score;
			if (LeftScore >= RightScore)
				return true;
			return false;
		});

	GameInstance->SetPlayerInfos(PlayerInfos);
}

UBar_B* AMainGameMode_B::GetBar() const
{
	return BarComponent;
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
			if (!(Character->GetState() == EState::EBeingHeld) && Character->IsAlive() && !bGameIsOver)	//this is an ugly fix. When a player is picked up, this is run and causes a lot of bugs otherwise.
				Character->Die();
		}
	}
}

void AMainGameMode_B::ResetMusic()
{
	if (GameInstance && (GameInstance->GetCurrentMusic() != Music))
	{
		GameInstance->SetAndPlayMusic(Music);
	}
}

void AMainGameMode_B::SortPlayerControllersByScore(TArray<APlayerController_B*>& TempPlayerControllers)
{
	TempPlayerControllers.Sort([&](const APlayerController_B& Left, const APlayerController_B& Right)
		{
			return Left.GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().Score > Right.GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().Score;
		});
}

TArray<APlayerController_B*> AMainGameMode_B::GetLeadingPlayerController()
{
	TArray<APlayerController_B*> TempPlayerControllers = PlayerControllers;
	SortPlayerControllersByScore(TempPlayerControllers);
	if (!TempPlayerControllers.IsValidIndex(0) || !TempPlayerControllers[0])
	{
		BError("Invalid Leader!");  return {};
	}

	TArray<APlayerController_B*> ControllersToRemove;
	const int HighestScore = TempPlayerControllers[0]->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->GetScoreValues().Score;
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
	
	LeaderFollower = GetWorld()->SpawnActor<ALeaderFollower_B>(BP_LeaderFollower, FTransform());
	if (!LeaderFollower)
		BError("Spawning LeaderFollower Failed!");
}

bool AMainGameMode_B::ShouldUseScoreMultiplier() const
{
	return bMultiplyScoresAgainstLeader;
}
