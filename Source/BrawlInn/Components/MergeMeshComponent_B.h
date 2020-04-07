// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/NoExportTypes.h"
#include "Engine/SkeletalMesh.h"
#include "MergeMeshComponent_B.generated.h"


class USkeletalMesh;
class UPhysicsAsset;
class USkeleton;
class UTexture2D;
class UMaterialInstanceDynamic;

struct FSkelMeshMergeUVTransforms;
struct FSkelMeshMergeSectionMapping;
/**
* Blueprint equivalent of FSkeleMeshMergeSectionMapping
* Info to map all the sections from a single source skeletal mesh to
* a final section entry in the merged skeletal mesh.
*/
USTRUCT(BlueprintType)
struct BRAWLINN_API FSkelMeshMergeSectionMapping_BP
{
    GENERATED_BODY()
        /** Indices to final section entries of the merged skeletal mesh */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Merge Params")
        TArray<int> SectionIDs;
};


/**
* Used to wrap a set of UV Transforms for one mesh.
*/

USTRUCT(BlueprintType)
struct BRAWLINN_API FSkelMeshMergeUVTransform
{
    GENERATED_BODY()
        /** A list of how UVs should be transformed on a given mesh, where index represents a specific UV channel. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Merge Params")
        TArray<FTransform> UVTransforms;
};
/**
* Blueprint equivalent of FSkelMeshMergeUVTransforms
* Info to map all the sections about how to transform their UVs
*/
USTRUCT(BlueprintType)
struct BRAWLINN_API FSkelMeshMergeUVTransformMapping
{
    GENERATED_BODY()
        /** For each UV channel on each mesh, how the UVS should be transformed. */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Merge Params")
        TArray<FSkelMeshMergeUVTransform> UVTransformsPerMesh;
};
/**
* Struct containing all parameters used to perform a Skeletal Mesh merge.
*/
USTRUCT(BlueprintType)
struct BRAWLINN_API FSkeletalMeshMergeParams
{
    GENERATED_BODY()
        FSkeletalMeshMergeParams()
    {
        MeshSectionMappings = {};
        UVTransformsPerMesh = {};
        StripTopLODS = 0;
        bNeedsCpuAccess = false;
        bSkeletonBefore = false;
        Skeleton = nullptr;
    }
    // An optional array to map sections from the source meshes to merged section entries
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<FSkelMeshMergeSectionMapping_BP> MeshSectionMappings;
    // An optional array to transform the UVs in each mesh
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<FSkelMeshMergeUVTransformMapping> UVTransformsPerMesh;
    // The list of skeletal meshes to merge.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<USkeletalMesh*> MeshesToMerge;
    // The number of high LODs to remove from input meshes
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 StripTopLODS;
    // Whether or not the resulting mesh needs to be accessed by the CPU for any reason (e.g. for spawning particle effects).
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        uint32 bNeedsCpuAccess : 1;
    // Update skeleton before merge. Otherwise, update after.
    // Skeleton must also be provided.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        uint32 bSkeletonBefore : 1;
    // Skeleton that will be used for the merged mesh.
    // Leave empty if the generated skeleton is OK.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
        class USkeleton* Skeleton;
};

USTRUCT()
struct BRAWLINN_API FMeshCategory
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TArray<USkeletalMesh*> Meshes;
};

USTRUCT()
struct BRAWLINN_API FTextureCategory
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FName ParameterName;

    UPROPERTY(EditAnywhere)
    TArray<UTexture2D*> Textures;
};

UCLASS()
class BRAWLINN_API UMergeMeshComponent_B : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMergeMeshComponent_B(const FObjectInitializer& ObjectInitializer);

    bool CreateRandomMesh();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;


protected:

    /**
* Merges the given meshes into a single mesh.
* @return The merged mesh (will be invalid if the merge failed).
*/
    UFUNCTION(BlueprintCallable, Category = "Mesh Merge", meta = (UnsafeDuringActorConstruction = "true"))
        USkeletalMesh* MergeMeshes(const FSkeletalMeshMergeParams& Params);

    void AddRandomMeshFromCategory(FSkeletalMeshMergeParams& params, FMeshCategory MeshCategory);

    void AddRandomTextureFromCategory(UMaterialInstanceDynamic* Material, FTextureCategory TextureCategory);
#if WITH_EDITOR
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
    void ToMergeParams(const TArray<FSkelMeshMergeSectionMapping_BP>& InSectionMappings, TArray<FSkelMeshMergeSectionMapping>& OutSectionMappings);
    
    void ToMergeParams(const TArray<FSkelMeshMergeUVTransformMapping>& InUVTransformsPerMesh, TArray<FSkelMeshMergeUVTransforms>& OutUVTransformsPerMesh);

    
    UPROPERTY(EditDefaultsOnly, Category = "Modular Mesh Pieces")
    USkeleton* Skeleton;

    UPROPERTY(EditDefaultsOnly, Category = "Modular Mesh Pieces")
    UPhysicsAsset* PhysicsAsset;

    UPROPERTY(EditDefaultsOnly, Category = "Modular Mesh Pieces")
    UMaterial* BaseMaterial;

    UPROPERTY(EditAnywhere, Category = "Modular Mesh Pieces")
    TArray<FMeshCategory> MeshCategories;

    UPROPERTY(EditAnywhere, Category = "Modular Mesh Pieces")
    TArray<FTextureCategory> TextureCategories;

    // ********** Randomizing **********
    UPROPERTY(EditDefaultsOnly, Category = "Scale Variation")
        float MinHeight = 0.7f;

    UPROPERTY(EditDefaultsOnly, Category = "Scale Variation")
        float MaxHeight= 1.05f;

    UPROPERTY(EditDefaultsOnly, Category = "Scale Variation")
        float MinWidth = 0.7f;

    UPROPERTY(EditDefaultsOnly, Category = "Scale Variation")
        float MaxWidth = 1.05f;

    // ********** Generation Time **********
    UPROPERTY(EditAnywhere, Category = "Generation Time")
    bool Generate = false;

public:
    UPROPERTY(EditAnywhere, Category = "Generation Time")
    bool bRandomizeOnBeginPlay = true;



};
