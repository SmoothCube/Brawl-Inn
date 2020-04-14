// Fill out your copyright notice in the Description page of Project Settings.


#include "MergeMeshComponent_B.h"
#include "SkeletalMeshMerge.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"


#include "BrawlInn.h"
#include "Characters/Character_B.h"

// Sets default values for this component's properties
UMergeMeshComponent_B::UMergeMeshComponent_B(const FObjectInitializer& ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UMergeMeshComponent_B::BeginPlay()
{
	Super::BeginPlay();
}

bool UMergeMeshComponent_B::CreateRandomMesh()
{
    if (!Skeleton) 
    { 
        ACharacter_B* Character = Cast<ACharacter_B>(GetOwner());
        if (Character)
        {
            BError("No Skeleton Selected for character %s!", *GetNameSafe(Character));
        }
        else
        {
            BError("No Skeleton Selected! Owning character invalid!");
        }
        return false;
    }

    //Randomize mesh
    FSkeletalMeshMergeParams params;
    params.bSkeletonBefore = true;
    params.Skeleton = Skeleton;

    for (FMeshCategory& category : MeshCategories)
        AddRandomMeshFromCategory(params, category);

    USkeletalMesh* MergedMesh = MergeMeshes(params);
    
    if (!IsValid(MergedMesh)){ BError("Merged Mesh is invalid!"); return false; }

    ACharacter_B* Character = Cast<ACharacter_B>(GetOwner());
    if (Character)
    {
        Character->GetMesh()->SetSkeletalMesh(MergedMesh);
        Character->GetMesh()->SkeletalMesh->Skeleton = Skeleton;
        //Randomize textures
        UMaterialInstanceDynamic* NewMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        for (FTextureCategory& category : TextureCategories)
            AddRandomTextureFromCategory(NewMaterialInstance, category);
        if (IsValid(NewMaterialInstance))
            Character->GetMesh()->SetMaterial(0,NewMaterialInstance);
        if (IsValid(PhysicsAsset))
        {
            if (GetWorld())
                Character->GetMesh()->SetPhysicsAsset(PhysicsAsset);
            else
                BError("World is invalid!");
        }
        //Randomize scale
        float Height = FMath::FRandRange(MinHeight, MaxHeight);
        float Width = FMath::FRandRange(MinWidth, MaxWidth);
        Character->SetActorScale3D({ Width,  Width, Height });

        return true;
    }
    return false;
}

void UMergeMeshComponent_B::AddRandomMeshFromCategory(FSkeletalMeshMergeParams& params, FMeshCategory MeshCategory)
{
    //finds a random mesh in array;'

    if (MeshCategory.Meshes.Num() == 0)
        return;

    int i = FMath::RandRange(0, MeshCategory.Meshes.Num() - 1);

    if (!MeshCategory.Meshes.IsValidIndex(i) || !MeshCategory.Meshes[i])
        return;

    //Adds it to the parameter;
    params.MeshesToMerge.Add(MeshCategory.Meshes[i]);
}

void UMergeMeshComponent_B::AddRandomTextureFromCategory(UMaterialInstanceDynamic* Material, FTextureCategory TextureCategory)
{
    //finds a random mesh in array;
    if (TextureCategory.Textures.Num() == 0)
        return;

    int i = FMath::RandRange(0, TextureCategory.Textures.Num() - 1);

    if (!TextureCategory.Textures.IsValidIndex(i) || !TextureCategory.Textures[i])
        return;

    Material->SetTextureParameterValue(TextureCategory.ParameterName, TextureCategory.Textures[i]);
}

#if WITH_EDITOR     
void UMergeMeshComponent_B::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    if (Generate == false) { return; } //if anything else thatn Generate was edited

    CreateRandomMesh();
    Generate = false;
}
#endif


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

    //Validates the meshes
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

    //checks if the meshes need CPU access
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


