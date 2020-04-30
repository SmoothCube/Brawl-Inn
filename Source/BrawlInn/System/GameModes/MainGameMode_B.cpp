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
#include "Characters/Player/RespawnPawn_B.h"

AMainGameMode_B::AMainGameMode_B()
{
	BarComponent = CreateDefaultSubobject<UBar_B>("BarComponent");
}

void AMainGameMode_B::BeginPlay()
{
	Super::BeginPlay();
	FullGameTime = GameTimeRemaining;
}

void AMainGameMode_B::Tick(float DeltaTime)
{
	CurrentTime += DeltaTime;
	if (!PlayerSpawnQueue.IsEmpty() && (CurrentTime > Delay))
	{
		CurrentTime = 0.f;
		FPlayerInfo info = *PlayerSpawnQueue.Peek();
		PlayerSpawnQueue.Pop();
		AActor* Pawn = SpawnRespawnPawn(info);
		if(PlayerSpawnQueue.IsEmpty() && IsValid(Pawn))
		{
			//LastRespawnPawnDestroyed(Pawn);
			Pawn->OnDestroyed.AddDynamic(this, &AMainGameMode_B::LastRespawnPawnDestroyed);
		}
	}
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

	for (FPlayerInfo Info : GameInstance->GetPlayerInfos())
	{
		AGamePlayerController_B* PlayerController = Cast<AGamePlayerController_B>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), Info.ID));
		if (!PlayerController) { BError("PlayerController for id %i not found. Check IDs in GameInstance", Info.ID); continue; }

		PlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->ResetScoreValues();
		PlayerController->GetLocalPlayer()->GetSubsystem<UScoreSubSystem_B>()->OnScoreChangedNoParam.AddUObject(this, &AMainGameMode_B::CallOnAnyScoreChange);
		PlayerSpawnQueue.Enqueue(Info);
		//SpawnCharacter(Info, false, FTransform());
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
	//if (GameInstance)
	//	GameInstance->PlayAnnouncerLine();
	//Swap camera after 1 second
	BI::Delay(this, 1, [&]()
		{
			UpdateViewTargets(nullptr, 1, true);
		});
}

void AMainGameMode_B::PregameCountdown()
{
	check(IsValid(Countdown));

	if (GameInstance)
		GameInstance->PlayAnnouncerLine(Countdown);
//	UGameplayStatics::PlaySound2D(GetWorld(), Countdown, 1.f, 1.0f);

	// Start game when sound is finished
	BWarn("Countdown time: %f", Countdown->GetDuration() - 0.75f);
	BI::Delay(this, Countdown->GetDuration() - 0.75f, [&]() {StartGame(); });
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

	if (GameTimeRemaining == TimeRemainingBeforeMultipleScore)
		StartMultipleScore();

	CheckTimeVoicelines();

	if (GameTimeRemaining <= 0)
		PostGame();

}

void AMainGameMode_B::CheckTimeVoicelines()
{
	if (!GameInstance) return;

	if(GameTimeRemaining == 1)
			GameInstance->PlayAnnouncerLine(OneSecondRemaining);

	if (GameTimeRemaining == 2)
		GameInstance->PlayAnnouncerLine(TwoSecondsRemaining);

	if (GameTimeRemaining == 3)
		GameInstance->PlayAnnouncerLine(ThreeSecondsRemaining);

	if (GameTimeRemaining == 4)
		GameInstance->PlayAnnouncerLine(FourSecondsRemaining);

	if (GameTimeRemaining == 5)
		GameInstance->PlayAnnouncerLine(FiveSecondsRemaining);

	if (GameTimeRemaining == 10)
		GameInstance->PlayAnnouncerLine(TenSecondsRemaining);

	if (GameTimeRemaining == 60)
		GameInstance->PlayAnnouncerLine(OneMinuteRemaining);

	if (GameTimeRemaining == FullGameTime/2)
		GameInstance->PlayAnnouncerLine(HalfwayPoint);
}

void AMainGameMode_B::StartMultipleScore()
{
	bMultipleScore = true;
	Overlay->DisplayText("Time is running out...", "DOUBLE POINTS!!", 3.f);
	if(GameInstance)
		GameInstance->PlayAnnouncerLine(DoublePoints);

}

bool AMainGameMode_B::MultipleScoreIsActivated() const
{
	return bMultipleScore;
}

void AMainGameMode_B::StartGame()
{
	OnGameStart_Delegate.Broadcast();

	UGameplayStatics::PlaySound2D(GetWorld(), StartSound, 1.f, 1.0f);

	Overlay->DisplayText("","LET THE BRAWL BEGIN!", 2.f);

	GetWorld()->GetTimerManager().SetTimer(TH_CountdownTimer, this, &AMainGameMode_B::UpdateClock, 1.f, true);

	GetWorld()->GetTimerManager().SetTimer(StartGameHandle, this, &AMainGameMode_B::StartMultiplyingScores, TimeBeforeMultiplyScoreAgainstLeader, false);

	EnableControllerInputs();

	if (GameInstance)
		GameInstance->SetAndPlayMusic(Music);
}

FGameStart& AMainGameMode_B::OnGameStart()
{
	return OnGameStart_Delegate;
}

void AMainGameMode_B::LastRespawnPawnDestroyed(AActor* DestroyedActor)
{
	BWarn("Starting Countdown");

	BI::Delay(this, 1, [&]() { PregameCountdown(); });

	//BI::Repeat(this, 1, 4, [&]() { PregameCountdownClock(); });
}

void AMainGameMode_B::PostGame()
{
	bGameIsOver = true;
	GetWorld()->GetTimerManager().PauseTimer(TH_CountdownTimer);
	//TODO PostGame needs to be implemented.

	DisableControllerInputs();
	LeaderFollower->Destroy();



	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACameraActor::StaticClass(), "EndGame", OutActors);
	check(OutActors.Num() != 0);
	ACameraActor* OutroCamera = Cast<ACameraActor>(OutActors[0]);

	GameCamera->SetActorTickEnabled(false);
	GameInstance->SetCameraSwapTransform(OutroCamera->GetActorTransform());
	
	GameInstance->PlayAnnouncerLine(RoundOver);

	Overlay->DisplayText("TIME'S UP!", "", 2.f);
	Overlay->HideScoreBoardAndClock();
	
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

FOnAnyScoreChange& AMainGameMode_B::OnAnyScoreChange()
{
	return OnAnyScoreChange_Delegate;
}

UBar_B* AMainGameMode_B::GetBar() const
{
	return BarComponent;
}

void AMainGameMode_B::CallOnAnyScoreChange() const
{
	OnAnyScoreChange_Delegate.Broadcast();
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
#if WITH_EDITOR

void AMainGameMode_B::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == NAME_None)
		return;
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AMainGameMode_B, bShowGameOverlay))
	{
		if (GetWorld()->IsPlayInEditor() && !bShowGameOverlay && Overlay)
			Overlay->SetVisibility(ESlateVisibility::Hidden);
		if (GetWorld()->IsPlayInEditor() && bShowGameOverlay && Overlay)
			Overlay->SetVisibility(ESlateVisibility::Visible);
	}
}
#endif

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
