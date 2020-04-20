// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "System/Structs/ScoreValues.h"
#include "VictoryScreenWidget_B.generated.h"

class UStatEntry_B;
class UGameInstance_B;
class UCharacterVictoryScreen_B;
class UTextBlock;
class UButton_B;

USTRUCT()
struct FCountNumber
{
	GENERATED_BODY()
public:
	FCountNumber() {  };
	FCountNumber(const int EndIn, const float DurationIn, UTextBlock* Text) : End(EndIn), Duration(DurationIn), TextBlock(Text) {}

	int Start = 0;
	int End = 0;
	float CurrentTime = 0.f;
	float Duration = 1.f;
	UTextBlock* TextBlock = nullptr;
};

USTRUCT()
struct FCountNumberQueue
{
	GENERATED_BODY()
		FCountNumberQueue() {}

	void Push(const FCountNumber Number) { Queue.Add(Number); }
	FCountNumber& Top() { if (Queue.IsValidIndex(0)) return Queue[0]; return Dummy; }
	void Pop() { Queue.RemoveAt(0); }
	int Size() const { return Queue.Num(); }
private:
	TArray<FCountNumber> Queue;
	FCountNumber Dummy;
};

UCLASS()
class BRAWLINN_API UVictoryScreenWidget_B : public UUserWidget
{
	GENERATED_BODY()
protected:

	virtual void NativeOnInitialized() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
		void ContinueButtonClicked();

	UFUNCTION(BlueprintCallable)
		void EnableContinueButton();

	void DisplayScores(EScoreValueTypes Type);

	void AddToCountQueue(int Start, int End, UTextBlock* TextBlock, int PlayerControllerID);

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton_B* ContinueButton;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UCharacterVictoryScreen_B* StatTopLeft;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UCharacterVictoryScreen_B* StatTopRight;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UCharacterVictoryScreen_B* StatBotLeft;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UCharacterVictoryScreen_B* StatBotRight;

	// ********** Arrays of Stats **********
	UPROPERTY(BlueprintReadWrite)
		TArray<UCharacterVictoryScreen_B*> StatBoards;

	UPROPERTY()
		UGameInstance_B* GameInstance = nullptr;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UStatEntry_B> BP_StatEntry;

	TArray<FCountNumberQueue> CountNumbers;
};
