// Fill out your copyright notice in the Description page of Project Settings.


#include "ToMainMenuConfirmation_B.h"

#include "Kismet/GameplayStatics.h"
#include "UI/UIElements/Button_B.h"

void UToMainMenuConfirmation_B::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	YesButton->OnClicked.AddDynamic(this, &UToMainMenuConfirmation_B::OnYesButtonClicked);
	NoButton->OnClicked.AddDynamic(this, &UToMainMenuConfirmation_B::OnNoButtonClicked);
}

void UToMainMenuConfirmation_B::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	YesButton->Execute_Tick(YesButton);
	NoButton->Execute_Tick(NoButton);
}

UButton_B* UToMainMenuConfirmation_B::GetNoButton() const
{
	return NoButton;
}

void UToMainMenuConfirmation_B::OnYesButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), *GetWorld()->GetCurrentLevel()->GetName());
}

void UToMainMenuConfirmation_B::OnNoButtonClicked()
{
	SetVisibility(ESlateVisibility::Hidden);

	const FInputModeGameOnly InputMode;
	GetOwningPlayer()->SetInputMode(InputMode);
}
