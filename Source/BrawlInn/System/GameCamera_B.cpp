// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCamera_B.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AGameCamera_B::AGameCamera_B()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(Scene);

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);

}

// Called when the game starts or when spawned
void AGameCamera_B::BeginPlay()
{
	Super::BeginPlay();

	SetActorLocation(FVector(0,0,50));
	SpringArm->TargetArmLength = SpringArmLength;
	
}

// Called every frame
void AGameCamera_B::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

