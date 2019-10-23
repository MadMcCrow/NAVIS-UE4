// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#pragma once

#include "Components/MeshComponent.h"
#include "Interfaces/Interface_CollisionDataProvider.h"
#include "GeneratedMeshComponent.generated.h"

USTRUCT(NoExport, Atomic)
struct FGeneratedTriangle
{
protected:

	UPROPERTY()
	FVector Vertex0;

	UPROPERTY()
	FVector Vertex1;

	UPROPERTY()
	FVector Vertex2;

public:

	FGeneratedTriangle(FVector v0 = FVector::ZeroVector, FVector v1 = FVector::ZeroVector, FVector v2 = FVector::ZeroVector)
		: Vertex0(v0), Vertex1(v1), Vertex2(v2)
	{
	}

	FVector &operator[](int32 idx) 
	{ 
		switch (idx)
		{
			default	:
			case 0	:
			return Vertex0;
			case 1	:
			return Vertex1;
			case 2	:
			return Vertex2;
		};
	}

	FVector V(int32 idx) const
	{ 
		switch (idx)
		{
			default	:
			case 0	:
			return Vertex0;
			case 1	:
			return Vertex1;
			case 2	:
			return Vertex2;
		};
	}

	FVector operator[](int32 idx) const
	{ 
		return V(idx);
	}

};

/**
 *	Component that allows you to specify custom triangle mesh geometry
 *	We only expose overrides to other modules
 */
UCLASS(minimalAPI)
class UGeneratedMeshComponent : public UMeshComponent, public IInterface_CollisionDataProvider
{
	GENERATED_BODY()

public:

	UGeneratedMeshComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Set the geometry to use on this triangle mesh */
	bool SetGeneratedMeshTriangles(const TArray<FGeneratedTriangle>& Triangles);

	/** Description of collision */
	UPROPERTY(BlueprintReadOnly, Category = "Collision")
		class UBodySetup* ModelBodySetup;

	// Begin UMeshComponent interface.
	NAVIS_CUSTOMMESH_API virtual int32 GetNumMaterials() const override;
	// End UMeshComponent interface.

private :
	// Begin Interface_CollisionDataProvider Interface
	NAVIS_CUSTOMMESH_API virtual bool GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) override;
	NAVIS_CUSTOMMESH_API virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
	NAVIS_CUSTOMMESH_API virtual bool WantsNegXTriMesh() override { return false; }
	// End Interface_CollisionDataProvider Interface

public :
	// Begin UPrimitiveComponent interface.
	NAVIS_CUSTOMMESH_API virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	NAVIS_CUSTOMMESH_API virtual class UBodySetup* GetBodySetup() override;
	// End UPrimitiveComponent interface.

	void UpdateBodySetup();
	void UpdateCollision();
private:



	// Begin USceneComponent interface.
	NAVIS_CUSTOMMESH_API virtual FBoxSphereBounds CalcBounds(const FTransform & LocalToWorld) const override;
	// Begin USceneComponent interface.

	/** Array of the triangle we created */
	TArray<FGeneratedTriangle> GeneratedMeshTris;

	friend class FGeneratedMeshSceneProxy;
};