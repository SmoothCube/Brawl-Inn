// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldUseable_B.h"

#include "BrawlInn.h"

void AShieldUseable_B::Use_Implementation()
{
	Super::Use_Implementation();

	BScreen("Shield");
}