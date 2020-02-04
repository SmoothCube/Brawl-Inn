// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthWidget_B.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/ProgressBar.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "BrawlInn.h"

void UHealthWidget_B::NativeConstruct()
{
	Super::NativeConstruct();

	//UImage* image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), "Barrel");
	//image->SetBrush(brush);

	//UProgressBar* ProgressBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), "Progressbar");
	//

	//UVerticalBox* Box = Cast<UVerticalBox>(GetWidgetFromName("Box"));
	//Box->AddChildToVerticalBox(ProgressBar);
	//Box->AddChildToVerticalBox(image);




}

bool UHealthWidget_B::Initialize()
{
	bool s = Super::Initialize();

	return s;
}

void UHealthWidget_B::UpdateHealthAmount(int Amount)
{
	BScreen("Update");
}
