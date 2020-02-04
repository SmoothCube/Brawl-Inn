// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterSelectionComponent_B.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BrawlInn.h"

#include "Characters/Player/PlayerCharacter_B.h"
#include "Characters/Player/PlayerController_B.h"
#include "System/MenuGameMode_B.h"

UCharacterSelectionComponent_B::UCharacterSelectionComponent_B()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterSelectionComponent_B::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> TCharacters;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerCharacter_B::StaticClass(), FName("Selection"), TCharacters);

	if (TCharacters.Num() == 0)
	{
		DestroyComponent();
		return;
	}
	for (const auto& Character : TCharacters)
	{
		Characters.Add(Cast<APlayerCharacter_B>(Character));
		StartLocations.Add(Cast<APlayerCharacter_B>(Character), Character->GetActorTransform());
	}

	CharacterSelected = Characters[CurrentIndex];

	BLog("Found %i characters", Characters.Num());

}
void UCharacterSelectionComponent_B::SelectCharacter(APlayerController_B* PlayerController)
{
	if (!IsValid(PlayerController))
		return;

	PlayerController->Possess(CharacterSelected);
	PlayerController->PlayerCharacter = CharacterSelected;

	//Vet ikke hvorfor jeg må gjøre dette, hjelper egt ikke tror jeg
	AMenuGameMode_B* GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->UpdateViewTarget(PlayerController);
	}

	SelectedCharacters.Add(CharacterSelected);
	Characters.Remove(CharacterSelected);
	NextCharacter(PlayerController);
	CharacterSelected_D.Execute();
}

void UCharacterSelectionComponent_B::Unselect(APlayerController_B* PlayerController)
{
	if (!PlayerController->HasValidCharacter())
		return;

	PlayerController->UnPossess();
	CharacterSelected = PlayerController->PlayerCharacter;
	PlayerController->PlayerCharacter = nullptr;

	Characters.Add(CharacterSelected);
	SelectedCharacters.Remove(CharacterSelected);


	for (const auto& Element : StartLocations)
	{
		if (Characters.Find(Element.Key) != INDEX_NONE)
			Element.Key->SetActorTransform(Element.Value);
	}

	//Vet ikke hvorfor jeg må gjøre dette, hjelper egt ikke tror jeg
	AMenuGameMode_B* GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->UpdateViewTarget(PlayerController);
	}

	Characters.Sort([](const APlayerCharacter_B& Left, const APlayerCharacter_B& Right) {
		return Left.GetActorLocation().X > Right.GetActorLocation().X;
		});

	bFirstTime = true;
	if (CharacterUnselected_D.IsBound())
		CharacterUnselected_D.Execute();
}

void UCharacterSelectionComponent_B::NextCharacter(APlayerController_B* PlayerController)
{
	AMenuGameMode_B* GameMode = Cast<AMenuGameMode_B>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
		GameMode->UpdateViewTarget(PlayerController);

	if (!bFirstTime)
		CharacterSelected->AddActorLocalRotation(FRotator(0, 180, 0));
	if (Characters.IsValidIndex(CurrentIndex + 1))
		CurrentIndex++;
	else
		CurrentIndex = 0;

	CharacterSelected = Characters[CurrentIndex];

	if (CharacterSelected)
		CharacterSelected->AddActorLocalRotation(FRotator(0, 180, 0));
	else
		BError("Character Selected is invalid");

	bFirstTime = false;
	BLog("Character selected: %s", *GetNameSafe(CharacterSelected));
}

