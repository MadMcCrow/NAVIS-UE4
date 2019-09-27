// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#include "NAVISPhysicsStatics.h"
#include "NAVIS_PhysicsPCH.h"


#if WITH_PHYSX
float UNAVISPhysicsStatics::GetConvexTruncatedVolume(const physx::PxConvexMesh* ConvexMesh,  const FVector &PlaneRelativePosition, const FVector &PlaneNormal );
{
	float Volume = 0.0f;
	const FVector PlaneNormalSafe = (PlaneNormal.Size() == 0.f) ? FVector::UpVector : PlaneNormal.GetUnsafeNormal() ;

	if (ConvexMesh != NULL)
	{
		// Preparation for convex mesh scaling implemented in another changelist
		FTransform ScaleTransform = FTransform(FQuat::Identity, FVector::ZeroVector, Scale);

		int32 NumPolys = ConvexMesh->getNbPolygons();
		PxHullPolygon PolyData;

		const PxVec3* Vertices = ConvexMesh->getVertices();
		const PxU8* Indices = ConvexMesh->getIndexBuffer();

		TArray<FVector> AddedVertices;

		for (int32 PolyIdx = 0; PolyIdx < NumPolys; ++PolyIdx)
		{
			if (ConvexMesh->getPolygonData(PolyIdx, PolyData))
			{
				for (int32 VertIdx = 2; VertIdx < PolyData.mNbVerts; ++ VertIdx)
				{
					// Grab triangle indices that we hit
					int32 I0 = Indices[PolyData.mIndexBase + 0];
					int32 I1 = Indices[PolyData.mIndexBase + (VertIdx - 1)];
					int32 I2 = Indices[PolyData.mIndexBase + VertIdx];

					// 
					// We have to determine if our points are under our plane or not
					auto IsUnderPlane = [PlaneRelativePosition, PlaneNormalSafe] ( const FVector &Position ) -> bool {
						const FVector Orient = Position - PlaneRelativePosition;
						return FVector::DotProduct(Orient, PlaneNormalSafe) < 0;
					 }

					const FVector V0 = P2UVector(Vertices[I0]);
					const FVector V1 = P2UVector(Vertices[I1]);
					const FVector V2 = P2UVector(Vertices[I2]);

					const bool I0UnderPlane = IsUnderPlane(V0);
					const bool I1UnderPlane = IsUnderPlane(V1);
					const bool I2UnderPlane = IsUnderPlane(V2);

					//
					// We have four possibilities :

					//
					// Case 0 : All points are over the plane :
					if(!(I0UnderPlane && I1UnderPlane && I2UnderPlane))
						{
						continue; // ignore those points
						}
					//
					// Case 1 : All points are below the plane :
					if(I0UnderPlane && I1UnderPlane && I2UnderPlane)
					{
						Volume += SignedVolumeOfTriangle(	ScaleTransform.TransformPosition(V0), 
															ScaleTransform.TransformPosition(V1), 
															ScaleTransform.TransformPosition(V2));
						continue;
					}
					//
					// Case 2 : at least one point is under the plane :
					else
					{
						auto Intersection = [PlaneRelativePosition, PlaneNormalSafe] ( const FVector &A, const FVector &B ) -> FVector {
							const FVector Segment = B - A;
							if(Segment.Size == 0)
								return A;
							const FVector NSegment = Segment.GetUnsafeNormal();
							const FVector PlaneToA = A - PlaneRelativePosition;
							float S = FVector::DotProduct(PlaneNormalSafe, PlaneToA ) / FVector::DotProduct(PlaneNormalSafe, NSegment );
							return NSegment * S;
					 	}

						// we will always have two cuts 
						FVector CutA, CutB;

						// we call alpha a cut in (v0, v1) and (v0, v2) with 0 under, and neg-alpha when 0 is over
						// we call beta a cut in (v0, v1) and (v1, v2) with 1 under, and neg-beta when 1 is over
						// we call gamma a cut in (v0, v2) and (v2, v1) with 2 under, and neg-gamma when 2 is over

						enum ECutCase {alpha, beta, gamma, error};
						
						ECutCase CutCase = ECutCase::error;
						if ((I0UnderPlane && (!I1UnderPlane && !I2UnderPlane)) || (I1UnderPlane && I2UnderPlane && !I0UnderPlane))
							CutCase = ECutCase::alpha;
						else
						if ((I1UnderPlane && (!I0UnderPlane && !I2UnderPlane)) || (I0UnderPlane && I2UnderPlane && !I1UnderPlane))
								CutCase = ECutCase::beta;
						else 
						if ((I2UnderPlane && (!I0UnderPlane && !I1UnderPlane)) || (I1UnderPlane && I0UnderPlane && !I2UnderPlane))
							CutCase = ECutCase::gamma;
						
				

						switch (CutCase)
						{
						case ECutCase::alpha:
							{
								CutA = Intersection(V0,V1);
								CutB = Intersection(V0,V2);
								if(I0UnderPlane)
								{
									AddedVertices.Add(CutA, CutB);
									Volume += SignedVolumeOfTriangle(	ScaleTransform.TransformPosition(V0), 
																		ScaleTransform.TransformPosition(CutA), 
																		ScaleTransform.TransformPosition(CutB));
								}
								else
								{
									AddedVertices.Add(CutB, CutA);
									Volume += SignedVolumeOfTriangle(	ScaleTransform.TransformPosition(CutB), 
																		ScaleTransform.TransformPosition(CutA), 
																		ScaleTransform.TransformPosition(V1));

									Volume += SignedVolumeOfTriangle(	ScaleTransform.TransformPosition(CutB), 
																		ScaleTransform.TransformPosition(V1), 
																		ScaleTransform.TransformPosition(V2));
								}
							}
							break;

						case ECutCase::beta:
							{
								CutA = Intersection(V1,V0);
								CutB = Intersection(V1,V2);
								if(I1UnderPlane)
								{
									AddedVertices.Add(CutB, CutA);
									Volume += SignedVolumeOfTriangle(	ScaleTransform.TransformPosition(CutA), 
																		ScaleTransform.TransformPosition(V1), 
																		ScaleTransform.TransformPosition(CutB));
								}
								else
								{
									AddedVertices.Add(CutA, CutB);
									Volume += SignedVolumeOfTriangle(	ScaleTransform.TransformPosition(V0), 
																		ScaleTransform.TransformPosition(CutA), 
																		ScaleTransform.TransformPosition(V2));

									Volume += SignedVolumeOfTriangle(	ScaleTransform.TransformPosition(CutA), 
																		ScaleTransform.TransformPosition(CutB), 
																		ScaleTransform.TransformPosition(V2));
								}
							}
							break;

						case ECutCase::gamma:
							{
								CutA = Intersection(V2,V0);
								CutB = Intersection(V2,V1);
								if(I2UnderPlane)
								{
									AddedVertices.Add(CutB, CutA);
									Volume += SignedVolumeOfTriangle(	ScaleTransform.TransformPosition(V2), 
																		ScaleTransform.TransformPosition(CutB), 
																		ScaleTransform.TransformPosition(CutA));
								}
								else
								{
									AddedVertices.Add(CutA, CutB);
									Volume += SignedVolumeOfTriangle(	ScaleTransform.TransformPosition(V0), 
																		ScaleTransform.TransformPosition(V1), 
																		ScaleTransform.TransformPosition(CutB));

									Volume += SignedVolumeOfTriangle(	ScaleTransform.TransformPosition(CutB), 
																		ScaleTransform.TransformPosition(V1), 
																		ScaleTransform.TransformPosition(CutA));
								}
							}
							break;
						default:
						case ECutCase::error:
							{
								UE_LOG(LogNAVIS_Physics, Error, TEXT("wrong cut, not possible"));
								Volume += SignedVolumeOfTriangle(	ScaleTransform.TransformPosition(V0), 
																	ScaleTransform.TransformPosition(V1), 
																	ScaleTransform.TransformPosition(V2));
								continue;
							}
							break;
						}

						//
						// this is all for the cutting part

					} // for (int32 VertIdx = 2; VertIdx < PolyData.mNbVerts; ++ VertIdx)

						//
						// Let's start making faces with the hole 
				} 
			}
		}
	}
	return Volume;	
}
//#endif // WITH_PHYSX


float UNAVISPhysicsStatics::GetActorVolume(const AActor * in)
{
	if(!in)
		return 0.f;
	return GetPrimitiveVolume( Cast<UPrimitiveComponent>(in->GetRootComponent()));
} 

float UNAVISPhysicsStatics::GetPrimitiveVolume(const UPrimitiveComponent * in)
{
	if(!in)
		return 0.f;
	if(!in->BodyInstance.BodySetup.IsValid())
		return 0.f;
	return in->BodyInstance.BodySetup.Get()->GetVolume(FVector::OneVector);
}

float UNAVISPhysicsStatics::GetPrimitivePercentageAtLevel(const UPrimitiveComponent * in, float height, const FVector &PlaneNormal  )
{
	const FVector planeLimit = PlaneNormal * height;

}


