// Fill out your copyright notice in the Description page of Project Settings.


#include "MergeMeshComponent_B.h"
#include "SkeletalMeshMerge.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "PhysicsEngine/PhysicsAsset.h"

#include "BrawlInn.h"
#include "Characters/Character_B.h"

// Sets default values for this component's properties
UMergeMeshComponent_B::UMergeMeshComponent_B(const FObjectInitializer& ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UMergeMeshComponent_B::BeginPlay()
{
	Super::BeginPlay();
}

bool UMergeMeshComponent_B::CreateRandomMesh()
{
    if (!Skeleton) { BError("No Skeleton Selected!"); return false; }
    // ...
    FSkeletalMeshMergeParams params;
    params.Skeleton = Skeleton;

    for (FMeshCategory& category : MeshCategories)
        AddRandomInArray(params, category.Meshes);

    USkeletalMesh* MergedMesh = MergeMeshes(params);

    if (!IsValid(MergedMesh))
        return false;

    ACharacter_B* Character = Cast<ACharacter_B>(GetOwner());
    if (Character)
    {
        Character->GetMesh()->SetSkeletalMesh(MergedMesh);
        if (PhysicsAsset)
        {
            Character->GetMesh()->SetPhysicsAsset(PhysicsAsset);
        }
        return true;
    }
    return false;
}


void UMergeMeshComponent_B::ToMergeParams(const TArray<FSkelMeshMergeSectionMapping_BP>& InSectionMappings, TArray<FSkelMeshMergeSectionMapping>& OutSectionMappings)
{
    if (InSectionMappings.Num() > 0)
    {
        OutSectionMappings.AddUninitialized(InSectionMappings.Num());
        for (int32 i = 0; i < InSectionMappings.Num(); ++i)
        {
            OutSectionMappings[i].SectionIDs = InSectionMappings[i].SectionIDs;
        }
    }
};

void UMergeMeshComponent_B::ToMergeParams(const TArray<FSkelMeshMergeUVTransformMapping>& InUVTransformsPerMesh, TArray<FSkelMeshMergeUVTransforms>& OutUVTransformsPerMesh)
{
    if (InUVTransformsPerMesh.Num() > 0)
    {
        OutUVTransformsPerMesh.Empty();
        OutUVTransformsPerMesh.AddUninitialized(InUVTransformsPerMesh.Num());
        for (int32 i = 0; i < InUVTransformsPerMesh.Num(); ++i)
        {
            TArray<TArray<FTransform>>& OutUVTransforms = OutUVTransformsPerMesh[i].UVTransformsPerMesh;
            const TArray<FSkelMeshMergeUVTransform>& InUVTransforms = InUVTransformsPerMesh[i].UVTransformsPerMesh;
            if (InUVTransforms.Num() > 0)
            {
                OutUVTransforms.Empty();
                OutUVTransforms.AddUninitialized(InUVTransforms.Num());
                for (int32 j = 0; j < InUVTransforms.Num(); j++)
                {
                    OutUVTransforms[i] = InUVTransforms[i].UVTransforms;
                }
            }
        }
    }
};

USkeletalMesh* UMergeMeshComponent_B::MergeMeshes(const FSkeletalMeshMergeParams& Params)
{
    TArray<USkeletalMesh*> MeshesToMergeCopy = Params.MeshesToMerge;
    MeshesToMergeCopy.RemoveAll([](USkeletalMesh* InMesh)
        {
            return InMesh == nullptr;
        });
    if (MeshesToMergeCopy.Num() <= 1)
    {
        UE_LOG(LogTemp, Warning, TEXT("Must provide multiple valid Skeletal Meshes in order to perform a merge."));
        return nullptr;
    }
    EMeshBufferAccess BufferAccess = Params.bNeedsCpuAccess ?
        EMeshBufferAccess::ForceCPUAndGPU :
        EMeshBufferAccess::Default;
    TArray<FSkelMeshMergeSectionMapping> SectionMappings;
    TArray<FSkelMeshMergeUVTransforms> UvTransforms;
    ToMergeParams(Params.MeshSectionMappings, SectionMappings);
    ToMergeParams(Params.UVTransformsPerMesh, UvTransforms);
    bool bRunDuplicateCheck = false;
    USkeletalMesh* BaseMesh = NewObject<USkeletalMesh>();
    if (Params.Skeleton && Params.bSkeletonBefore)
    {
        BaseMesh->Skeleton = Params.Skeleton;
        bRunDuplicateCheck = true;
        for (USkeletalMeshSocket* Socket : BaseMesh->GetMeshOnlySocketList())
        {
            if (Socket)
            {
                UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocket: %s"), *(Socket->SocketName.ToString()));
            }
        }
        for (USkeletalMeshSocket* Socket : BaseMesh->Skeleton->Sockets)
        {
            if (Socket)
            {
                UE_LOG(LogTemp, Warning, TEXT("SkelSocket: %s"), *(Socket->SocketName.ToString()));
            }
        }
    }
    FSkeletalMeshMerge Merger(BaseMesh, MeshesToMergeCopy, SectionMappings, Params.StripTopLODS, BufferAccess, UvTransforms.GetData());
    if (!Merger.DoMerge())
    {
        UE_LOG(LogTemp, Warning, TEXT("Merge failed!"));
        return nullptr;
    }
    if (Params.Skeleton && !Params.bSkeletonBefore)
    {
        BaseMesh->Skeleton = Params.Skeleton;
    }
    if (bRunDuplicateCheck)
    {
        TArray<FName> SkelMeshSockets;
        TArray<FName> SkelSockets;
        for (USkeletalMeshSocket* Socket : BaseMesh->GetMeshOnlySocketList())
        {
            if (Socket)
            {
                SkelMeshSockets.Add(Socket->GetFName());
                UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocket: %s"), *(Socket->SocketName.ToString()));
            }
        }
        for (USkeletalMeshSocket* Socket : BaseMesh->Skeleton->Sockets)
        {
            if (Socket)
            {
                SkelSockets.Add(Socket->GetFName());
                UE_LOG(LogTemp, Warning, TEXT("SkelSocket: %s"), *(Socket->SocketName.ToString()));
            }
        }
        TSet<FName> UniqueSkelMeshSockets;
        TSet<FName> UniqueSkelSockets;
        UniqueSkelMeshSockets.Append(SkelMeshSockets);
        UniqueSkelSockets.Append(SkelSockets);
        int32 Total = SkelSockets.Num() + SkelMeshSockets.Num();
        int32 UniqueTotal = UniqueSkelMeshSockets.Num() + UniqueSkelSockets.Num();
        UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"), SkelMeshSockets.Num(), SkelSockets.Num(), Total);
        UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"), UniqueSkelMeshSockets.Num(), UniqueSkelSockets.Num(), UniqueTotal);
        UE_LOG(LogTemp, Warning, TEXT("Found Duplicates: %s"), *((Total != UniqueTotal) ? FString("True") : FString("False")));
    }
    return BaseMesh;
}

void UMergeMeshComponent_B::AddRandomInArray(FSkeletalMeshMergeParams& params, TArray<USkeletalMesh*> Array)
{
    //finds a random mesh in array;
    if (Array.Num() == 0)
        return;

    int i = FMath::RandRange(0, Array.Num()-1);
    
    if (!Array.IsValidIndex(i) || !Array[i])
        return;
    
    //Adds it to the parameter;
    params.MeshesToMerge.Add(Array[i]);
}
