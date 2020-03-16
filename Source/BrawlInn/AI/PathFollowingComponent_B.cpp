// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFollowingComponent_B.h"
#include "BrawlInn.h"
#include "DrawDebugHelpers.h"
#include "Characters/AI/AIController_B.h"
#include "Characters/Character_B.h"

void UPathFollowingComponent_B::BeginPlay()
{
	Super::BeginPlay();
}

/** sets variables related to current move segment */
void UPathFollowingComponent_B::SetMoveSegment(int32 SegmentStartIndex)
{
	Super::SetMoveSegment(SegmentStartIndex);

}

/** follow current path segment */
void UPathFollowingComponent_B::FollowPathSegment(float DeltaTime)
{
	Super::FollowPathSegment(DeltaTime);
	AAIController_B* Controller = Cast<AAIController_B>(GetOwner());
	ACharacter_B* Character = Cast<ACharacter_B>(Controller->GetPawn());
	Character->SetActorRotation(MoveSegmentDirection.ToOrientationRotator());
	//DrawDebugLine(GetWorld(), Character->GetActorLocation(), Character->GetActorLocation() + (MoveSegmentDirection.GetSafeNormal() * 200.f), FColor::Red, false, DeltaTime);

}

/** check state of path following, update move segment if needed */
void UPathFollowingComponent_B::UpdatePathSegment()
{
	Super::UpdatePathSegment();

}

/** called when owner is preparing new pathfinding request */
void UPathFollowingComponent_B::OnPathfindingQuery(FPathFindingQuery& Query)
{
	Super::OnPathfindingQuery(Query);

}

/** notify about finished movement */
void UPathFollowingComponent_B::OnPathFinished(const FPathFollowingResult& Result)
{
	Super::OnPathFinished(Result);
}