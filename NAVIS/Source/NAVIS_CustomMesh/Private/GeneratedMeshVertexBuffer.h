// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved
#pragma once

#include "NAVIS_CustomMeshPCH.h"
#include "DynamicMeshBuilder.h"

class FGeneratedMeshVertexBuffer : public FVertexBuffer
{
public:
	TArray<FDynamicMeshVertex> Vertices;

	virtual void InitRHI()
	{
		FRHIResourceCreateInfo CreateInfo;
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.Num() * sizeof(FDynamicMeshVertex), BUF_Static, CreateInfo);

		// Copy the vertex data into the vertex buffer.
		void* VertexBufferData = RHILockVertexBuffer(VertexBufferRHI, 0, Vertices.Num() * sizeof(FDynamicMeshVertex), RLM_WriteOnly);
		FMemory::Memcpy(VertexBufferData, Vertices.GetData(), Vertices.Num() * sizeof(FDynamicMeshVertex));
		RHIUnlockVertexBuffer(VertexBufferRHI);

	}

};

/** Index Buffer */
class FGeneratedMeshIndexBuffer : public FIndexBuffer
{
public:
	TArray<int32> Indices;

	virtual void InitRHI()
	{

		FRHIResourceCreateInfo CreateInfo;
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), Indices.Num() * sizeof(int32), BUF_Static, CreateInfo);

		// Write the indices to the index buffer.
		void* Buffer = RHILockIndexBuffer(IndexBufferRHI, 0, Indices.Num() * sizeof(int32), RLM_WriteOnly);
		FMemory::Memcpy(Buffer, Indices.GetData(), Indices.Num() * sizeof(int32));
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}
};

/** Vertex Factory */
class FGeneratedMeshVertexFactory : public FLocalVertexFactory
{
public:

	FGeneratedMeshVertexFactory(ERHIFeatureLevel::Type InFeatureLevel, const char* InDebugName) : FLocalVertexFactory(InFeatureLevel, InDebugName)
	{}

#if (PLATFORM_WINDOWS || PLATFORM_XBOXONE || PLATFORM_PS4)
	FGeneratedMeshVertexFactory() : FLocalVertexFactory(ERHIFeatureLevel::SM5, "NAVIS_MESH_GENERATOR")
	{}
#else 
	FGeneratedMeshVertexFactory() : FLocalVertexFactory(ERHIFeatureLevel::ES3_1, "NAVIS_MESH_GENERATOR")
	{}
#endif

	/** Initialization */
	void Init(const FGeneratedMeshVertexBuffer* VertexBuffer)
	{
		check(!IsInRenderingThread());

		// Fixed for 4.22
		FGeneratedMeshVertexFactory* This = this; // remove const
		ENQUEUE_RENDER_COMMAND(InitGeneratedMeshVertexFactory)(
			[This, &VertexBuffer](FRHICommandListImmediate& RHICmdList)
		{
			FDataType NewData;
			// Initialize the vertex factory's stream components.
			NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Position, VET_Float3);
			NewData.TextureCoordinates.Add(
				FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FDynamicMeshVertex, TextureCoordinate), sizeof(FDynamicMeshVertex), VET_Float2)
			);
			NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentX, VET_PackedNormal);
			NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentZ, VET_PackedNormal);
			NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Color, VET_Color);
			This->SetData(NewData);
		});
	}
};

