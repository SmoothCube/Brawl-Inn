// Fill out your copyright notice in the Description page of Project Settings.


#include "TankardDropPoint_B.h"

#include "Hazards/Bar_B.h"

void ATankardDropPoint_B::ItemDestroyed()
{
	if (Bar)
		Bar->StartRandomOrder();

	Destroy();
}
