// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "PathFollowingComponent_B.generated.h"


UCLASS()
class BRAWLINN_API UPathFollowingComponent_B : public UPathFollowingComponent
{
	GENERATED_BODY()

protected:

    virtual void BeginPlay() override;

    /** sets variables related to current move segment */
    virtual void SetMoveSegment(int32 SegmentStartIndex) override;

    /** follow current path segment */
    virtual void FollowPathSegment(float DeltaTime) override;

    /** check state of path following, update move segment if needed */
    virtual void UpdatePathSegment() override;

    /** called when owner is preparing new pathfinding request */
    virtual void OnPathfindingQuery(FPathFindingQuery& Query) override;

    /** notify about finished movement */
    virtual void OnPathFinished(const FPathFollowingResult& Result) override;
	
};
