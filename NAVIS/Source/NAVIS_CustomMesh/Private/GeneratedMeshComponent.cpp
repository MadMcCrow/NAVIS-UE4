// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#include "GeneratedMeshComponent.h"
#include "NAVIS_CustomMeshPCH.h"
#include "GeneratedMeshVertexBuffer.h"
#include "Materials/Material.h"




// UGeneratedMeshComponent class

UGeneratedMeshComponent::UGeneratedMeshComponent(const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UGeneratedMeshComponent::SetGeneratedMeshTriangles(const TArray<FGeneratedTriangle>& Triangles)
{
	GeneratedMeshTris = Triangles;

	UpdateCollision();

	// Need to recreate scene proxy to send it over
	MarkRenderStateDirty();

	return true;
}


FPrimitiveSceneProxy* UGeneratedMeshComponent::CreateSceneProxy()
{
	
	/** Scene proxy defined only inside the scope of this one function */
	class FGeneratedMeshSceneProxy : public FPrimitiveSceneProxy
	{
	private:

		UMaterialInterface* Material;
		FGeneratedMeshVertexBuffer VertexBuffer;
		FGeneratedMeshIndexBuffer IndexBuffer;
		FGeneratedMeshVertexFactory VertexFactory;
		FMaterialRelevance MaterialRelevance;

	public:

		FGeneratedMeshSceneProxy(UGeneratedMeshComponent* Component)	: FPrimitiveSceneProxy(Component)
#if (PLATFORM_WINDOWS || PLATFORM_XBOXONE || PLATFORM_PS4)
	, MaterialRelevance(Component->GetMaterialRelevance(ERHIFeatureLevel::SM5))
#else 
	, MaterialRelevance(Component->GetMaterialRelevance(ERHIFeatureLevel::ES3_1))
#endif
		{
			const FColor VertexColor(255,255,255);

			// Add each triangle to the vertex/index buffer
			for(int TriIdx=0; TriIdx<Component->GeneratedMeshTris.Num(); TriIdx++)
			{
				FGeneratedTriangle& Tri = Component->GeneratedMeshTris[TriIdx];

				const FVector Edge01 = (Tri[1] - Tri[0]);
				const FVector Edge02 = (Tri[2] - Tri[0]);

				const FVector TangentX = Edge01.GetSafeNormal();
				const FVector TangentZ = (Edge02 ^ Edge01).GetSafeNormal();
				const FVector TangentY = (TangentX ^ TangentZ).GetSafeNormal();

				FDynamicMeshVertex Vert0;
				Vert0.Position = Tri[0];
				Vert0.Color = VertexColor;
				Vert0.SetTangents(TangentX, TangentY, TangentZ);
				int32 VIndex = VertexBuffer.Vertices.Add(Vert0);
				IndexBuffer.Indices.Add(VIndex);

				FDynamicMeshVertex Vert1;
				Vert1.Position = Tri[1];
				Vert1.Color = VertexColor;
				Vert1.SetTangents(TangentX, TangentY, TangentZ);
				VIndex = VertexBuffer.Vertices.Add(Vert1);
				IndexBuffer.Indices.Add(VIndex);

				FDynamicMeshVertex Vert2;
				Vert2.Position = Tri[2];
				Vert2.Color = VertexColor;
				Vert2.SetTangents(TangentX, TangentY, TangentZ);
				VIndex = VertexBuffer.Vertices.Add(Vert2);
				IndexBuffer.Indices.Add(VIndex);
			}

			// Init vertex factory
			VertexFactory.Init(&VertexBuffer);

			// Enqueue initialization of render resource
			BeginInitResource(&VertexBuffer);
			BeginInitResource(&IndexBuffer);
			BeginInitResource(&VertexFactory);

			// Grab material
			Material = Component->GetMaterial(0);
			if(Material == NULL)
			{
				Material = UMaterial::GetDefaultMaterial(MD_Surface);
			}
		}

		virtual ~FGeneratedMeshSceneProxy()
		{
			VertexBuffer.ReleaseResource();
			IndexBuffer.ReleaseResource();
			VertexFactory.ReleaseResource();
		}

		virtual void DrawDynamicElements(FPrimitiveDrawInterface* PDI,const FSceneView* View)
		{
			// if(IsSelected())
			QUICK_SCOPE_CYCLE_COUNTER( STAT_GeneratedMeshSceneProxy_DrawDynamicElements );

			const bool bWireframe = View->Family->EngineShowFlags.Wireframe;

			// if(IsSelected())
			auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
				GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy() : NULL,
				FLinearColor(0, 0.5f, 1.f)
				);

			FMaterialRenderProxy* MaterialProxy = NULL;
			if(bWireframe)
			{
				MaterialProxy = WireframeMaterialInstance;
			}
			else
			{
				MaterialProxy = Material->GetRenderProxy();
			}

			// Draw the mesh.
			FMeshBatch Mesh;
			FMeshBatchElement& BatchElement = Mesh.Elements[0];
			BatchElement.IndexBuffer = &IndexBuffer;
			Mesh.bWireframe = bWireframe;
			Mesh.VertexFactory = &VertexFactory;
			Mesh.MaterialRenderProxy = MaterialProxy;
			// @todo might try to put DrawVelocity to true
			BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), GetLocalBounds(), true, false);

			BatchElement.FirstIndex = 0;
			BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
			BatchElement.MinVertexIndex = 0;
			BatchElement.MaxVertexIndex = VertexBuffer.Vertices.Num() - 1;
			Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
			Mesh.Type = PT_TriangleList;
			Mesh.DepthPriorityGroup = SDPG_World;
			PDI->DrawMesh(Mesh);
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = IsShown(View);
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bDynamicRelevance = true;
			MaterialRelevance.SetPrimitiveViewRelevance(Result);
			return Result;
		}

		virtual bool CanBeOccluded() const override
		{
			return !MaterialRelevance.bDisableDepthTest;
		}

		virtual uint32 GetMemoryFootprint( void ) const { return( sizeof( *this ) + GetAllocatedSize() ); }

		uint32 GetAllocatedSize( void ) const { return( FPrimitiveSceneProxy::GetAllocatedSize() ); }

		virtual SIZE_T GetTypeHash() const override {return 336103622995LL /*NAVIS in ASCII */; }
	
	};
	
	//Only create if have enough tris
	if(GeneratedMeshTris.IsValidIndex(0))
	{
		return new FGeneratedMeshSceneProxy(this);
	}
	else
	{
		return nullptr;
	}
}

int32 UGeneratedMeshComponent::GetNumMaterials() const
{
	return 1;
}


FBoxSphereBounds UGeneratedMeshComponent::CalcBounds(const FTransform & LocalToWorld) const
{

	auto min = [](const FVector &A, const FVector &B )-> FVector { 
			return FVector (FMath::Min(A.X,B.X), FMath::Min(A.Y,B.Y), FMath::Min(A.Z,B.Z));
			};
	
	auto max = [](const FVector &A, const FVector &B )-> FVector { 
			return FVector (FMath::Min(A.X,B.X), FMath::Min(A.Y,B.Y), FMath::Min(A.Z,B.Z));
			};

	FVector vecMin = min(min(GeneratedMeshTris[0].V(0), GeneratedMeshTris[0].V(1)),GeneratedMeshTris[0].V(2));
	FVector vecMax = max(max(GeneratedMeshTris[0].V(0), GeneratedMeshTris[0].V(1)),GeneratedMeshTris[0].V(2));
	// Get maximum and minimum X, Y and Z positions of vectors
	for (int32 TriIdx = 0; TriIdx < GeneratedMeshTris.Num(); TriIdx++)
	{
		vecMin = min(min(GeneratedMeshTris[0].V(0), GeneratedMeshTris[0].V(1)),min(GeneratedMeshTris[0].V(2),vecMin ));
		vecMax = max(max(GeneratedMeshTris[0].V(0), GeneratedMeshTris[0].V(1)),max(GeneratedMeshTris[0].V(2),vecMax ));
	}
	
	FVector vecOrigin = ((vecMax - vecMin) / 2) + vecMin;	/* Origin = ((Max Vertex's Vector - Min Vertex's Vector) / 2 ) + Min Vertex's Vector */
	FVector BoxPoint = vecMax - vecMin;			/* The difference between the "Maximum Vertex" and the "Minimum Vertex" is our actual Bounds Box */
	return FBoxSphereBounds(vecOrigin, BoxPoint, BoxPoint.Size()).TransformBy(LocalToWorld);
}


bool UGeneratedMeshComponent::GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData)
{
	FTriIndices Triangle;

	for(int32 i=0;i<GeneratedMeshTris.Num();i++) {
		const FGeneratedTriangle& tri = GeneratedMeshTris[i];

		Triangle.v0 = CollisionData->Vertices.Add(tri[0]);
		Triangle.v1 = CollisionData->Vertices.Add(tri[1]);
		Triangle.v2 = CollisionData->Vertices.Add(tri[2]);
		
		CollisionData->Indices.Add(Triangle);
		CollisionData->MaterialIndices.Add(i);
	}

	CollisionData->bFlipNormals = true;
	
	return true;
}

bool UGeneratedMeshComponent::ContainsPhysicsTriMeshData(bool InUseAllTriData) const
{
	return (GeneratedMeshTris.Num() > 0);
}

void UGeneratedMeshComponent::UpdateBodySetup() {
	if (ModelBodySetup == NULL)	{
		ModelBodySetup = NewObject<UBodySetup>(this, UBodySetup::StaticClass());
		ModelBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		ModelBodySetup->bMeshCollideAll = true;
	}
}

void UGeneratedMeshComponent::UpdateCollision() {
	if (bPhysicsStateCreated) {
		DestroyPhysicsState();
		UpdateBodySetup();
		CreatePhysicsState();

                ModelBodySetup->InvalidatePhysicsData(); //Will not work in Packaged build
                                                         //Epic needs to add support for this
		ModelBodySetup->CreatePhysicsMeshes();
	}
}

UBodySetup* UGeneratedMeshComponent::GetBodySetup() {
	UpdateBodySetup();
	return ModelBodySetup;
}