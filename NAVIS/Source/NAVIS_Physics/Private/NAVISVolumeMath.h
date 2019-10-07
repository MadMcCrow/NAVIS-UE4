// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#include "NAVIS_PhysicsPCH.h"
#include "Algo/Reverse.h"						 // to reverse TArray
#include "GenericPlatform/GenericPlatformMath.h" // To ceil
#include "Kismet/KismetMathLibrary.h"
#include "PhysXCookHelper.h" // PhysX

#if WITH_PHYSX
	#include "PhysXPublic.h"
	//#include "PhysicsEngine/PhysXSupport.h" // not necessary
#endif // WITH_PHYSX


/** 
 *	FNAVISVolumeMath struct used as a namespace to hold all functions related to Volume calculation in NAVIS
 */
struct FNAVISVolumeMath
{
private :
	// typedef for more readability
	typedef TPair<FVector,FVector> FSegment;

	// @see Engine\Source\Runtime\Engine\Private\PhysicsEngine\BodySetup.cpp
	// http://amp.ece.cmu.edu/Publication/Cha/icip01_Cha.pdf
	// http://stackoverflow.com/questions/1406029/how-to-calculate-the-volume-of-a-3d-mesh-object-the-surface-of-which-is-made-up
	static float SignedVolumeOfTriangle(const FVector& p1, const FVector& p2, const FVector& p3) 
	{
		return FVector::DotProduct(p1, FVector::CrossProduct(p2, p3)) / 6.0f;
	}

#if WITH_PHYSX
	// GetPhysXConvexTruncatedVolume()	works for all convex meshes as all are using physx Convex mesh
	static float GetPhysXConvexTruncatedVolume(physx::PxConvexMesh * ConvexMesh, const FVector &PlaneRelativePosition, const FVector &PlaneNormal, const FVector& Scale)
	{
		float Volume = 0.0f;
		
		const FVector PlaneNormalSafe = (PlaneNormal.Size() == 0.f) ? FVector::UpVector : PlaneNormal.GetUnsafeNormal();
		if (ConvexMesh != NULL )
		{
			// Preparation for convex mesh scaling implemented in another changelist
			FTransform ScaleTransform = FTransform(FQuat::Identity, FVector::ZeroVector, Scale);

			int32 NumPolys = ConvexMesh->getNbPolygons();
			PxHullPolygon PolyData;

			const PxVec3 *Vertices = ConvexMesh->getVertices();
			const PxU8 *Indices = ConvexMesh->getIndexBuffer();

			TArray<FVector>	AddedVertices;
			TArray<FSegment> AddedSegments;

			for (int32 PolyIdx = 0; PolyIdx < NumPolys; ++PolyIdx)
			{
				if (ConvexMesh->getPolygonData(PolyIdx, PolyData))
				{
					for (int32 VertIdx = 2; VertIdx < PolyData.mNbVerts; ++VertIdx)
					{
						// Grab triangle indices that we hit
						int32 I0 = Indices[PolyData.mIndexBase + 0];
						int32 I1 = Indices[PolyData.mIndexBase + (VertIdx - 1)];
						int32 I2 = Indices[PolyData.mIndexBase + VertIdx];

						//
						// We have to determine if our points are under our plane or not
						auto IsUnderPlane = [&PlaneRelativePosition, &PlaneNormalSafe](const FVector &Position) -> bool {
							const FVector Orient = Position - PlaneRelativePosition;
							return FVector::DotProduct(Orient, PlaneNormalSafe) < 0;
						};

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
						if (!(I0UnderPlane && I1UnderPlane && I2UnderPlane))
						{
							continue; // ignore those points
						}
						//
						// Case 1 : All points are below the plane :
						if (I0UnderPlane && I1UnderPlane && I2UnderPlane)
						{
							Volume += SignedVolumeOfTriangle(ScaleTransform.TransformPosition(V0),
															 ScaleTransform.TransformPosition(V1),
															 ScaleTransform.TransformPosition(V2));
							continue;
						}
						//
						// Case 2 : at least one point is under the plane :
						else
						{
							auto Intersection = [&PlaneRelativePosition, &PlaneNormalSafe](const FVector &A, const FVector &B) -> FVector {
								const FVector Segment = B - A;
								if (Segment.Size() == 0)
									return A;
								const FVector NSegment = Segment.GetUnsafeNormal();
								const FVector PlaneToA = A - PlaneRelativePosition;
								float S = FVector::DotProduct(PlaneNormalSafe, PlaneToA) / FVector::DotProduct(PlaneNormalSafe, NSegment);
								return NSegment * S;
							};

							// we will always have two cuts
							FVector CutA,
								 CutB;

							// we call alpha a cut in (v0, v1) and (v0, v2) with 0 under, and neg-alpha when 0 is over
							// we call beta a cut in (v0, v1) and (v1, v2) with 1 under, and neg-beta when 1 is over
							// we call gamma a cut in (v0, v2) and (v2, v1) with 2 under, and neg-gamma when 2 is over

							enum ECutCase
							{
								alpha,
								beta,
								gamma,
								error
							};

							ECutCase CutCase = ECutCase::error;
							if ((I0UnderPlane && (!I1UnderPlane && !I2UnderPlane)) || (I1UnderPlane && I2UnderPlane && !I0UnderPlane))
								CutCase = ECutCase::alpha;
							else if ((I1UnderPlane && (!I0UnderPlane && !I2UnderPlane)) || (I0UnderPlane && I2UnderPlane && !I1UnderPlane))
								CutCase = ECutCase::beta;
							else if ((I2UnderPlane && (!I0UnderPlane && !I1UnderPlane)) || (I1UnderPlane && I0UnderPlane && !I2UnderPlane))
								CutCase = ECutCase::gamma;

							auto AddVertices = [&AddedSegments, &AddedVertices](FVector &A, FVector &B) {
								AddedSegments.Add(FSegment(A, B));
								AddedVertices.AddUnique(A);
								AddedVertices.AddUnique(B);
							};

							switch (CutCase)
							{
							case ECutCase::alpha:
							{
								CutA = Intersection(V0, V1);
								CutB = Intersection(V0, V2);
								if (I0UnderPlane)
								{
									AddVertices(CutA, CutB);
									Volume += SignedVolumeOfTriangle(ScaleTransform.TransformPosition(V0),
																	 ScaleTransform.TransformPosition(CutA),
																	 ScaleTransform.TransformPosition(CutB));
								}
								else // neg alpha
								{
									AddVertices(CutB, CutA);
									Volume += SignedVolumeOfTriangle(ScaleTransform.TransformPosition(CutB),
																	 ScaleTransform.TransformPosition(CutA),
																	 ScaleTransform.TransformPosition(V1));

									Volume += SignedVolumeOfTriangle(ScaleTransform.TransformPosition(CutB),
																	 ScaleTransform.TransformPosition(V1),
																	 ScaleTransform.TransformPosition(V2));
								}
							}
							break;

							case ECutCase::beta:
							{
								CutA = Intersection(V1, V0);
								CutB = Intersection(V1, V2);
								if (I1UnderPlane)
								{
									AddVertices(CutB, CutA);
									Volume += SignedVolumeOfTriangle(ScaleTransform.TransformPosition(CutA),
																	 ScaleTransform.TransformPosition(V1),
																	 ScaleTransform.TransformPosition(CutB));
								}
								else // neg beta
								{
									AddVertices(CutA, CutB);
									Volume += SignedVolumeOfTriangle(ScaleTransform.TransformPosition(V0),
																	 ScaleTransform.TransformPosition(CutA),
																	 ScaleTransform.TransformPosition(V2));

									Volume += SignedVolumeOfTriangle(ScaleTransform.TransformPosition(CutA),
																	 ScaleTransform.TransformPosition(CutB),
																	 ScaleTransform.TransformPosition(V2));
								}
							}
							break;

							case ECutCase::gamma:
							{
								CutA = Intersection(V2, V0);
								CutB = Intersection(V2, V1);
								if (I2UnderPlane)
								{
									AddVertices(CutB, CutA);
									Volume += SignedVolumeOfTriangle(ScaleTransform.TransformPosition(V2),
																	 ScaleTransform.TransformPosition(CutB),
																	 ScaleTransform.TransformPosition(CutA));
								}
								else // neg gamma
								{
									AddVertices(CutA, CutB);
									Volume += SignedVolumeOfTriangle(ScaleTransform.TransformPosition(V0),
																	 ScaleTransform.TransformPosition(V1),
																	 ScaleTransform.TransformPosition(CutB));

									Volume += SignedVolumeOfTriangle(ScaleTransform.TransformPosition(CutB),
																	 ScaleTransform.TransformPosition(V1),
																	 ScaleTransform.TransformPosition(CutA));
								}
							}
							break;
							default:
							case ECutCase::error:
							{
								UE_LOG(LogNAVIS_Physics, Error, TEXT("wrong cut, not possible"));
								Volume += SignedVolumeOfTriangle(ScaleTransform.TransformPosition(V0),
																 ScaleTransform.TransformPosition(V1),
																 ScaleTransform.TransformPosition(V2));
								continue;
							}
							break;
							}

							// this is all for the cutting part
						}

					} // end of for (int32 VertIdx = 2; VertIdx < PolyData.mNbVerts; ++ VertIdx)

					// If we dont have any points next step makes no sens
					if (!AddedVertices.IsValidIndex(0))
						return Volume;

					// find iso-centroid
					FVector centroid = FVector::ZeroVector;
					for (auto point : AddedVertices)
						centroid += point;
					centroid /= AddedVertices.Num();

					auto Ref = AddedVertices[0];
					//
					// Sort Vertices by orientation, this is only possible because we're in a convex shape
					AddedVertices.Sort([&PlaneNormal, &centroid, &Ref](const FVector &A, const FVector &B) {
						if (A == Ref)
							return true;
						if (B == Ref)
							return false;
						const auto D = FVector::CrossProduct(A - centroid, B - centroid);
						return FVector::DotProduct(D, PlaneNormal) < 0.f;
					});

					// we need to make sure the array is in correct order compared to stored segments
					// this could be parrallelized
					bool bInvert = false;
					for (int idx = 1; idx < AddedVertices.Num(); idx++)
					{
						if (AddedSegments.Find(FSegment(AddedVertices[idx - 1], AddedVertices[idx])) != INDEX_NONE)
						{
							bInvert = true;
							break;
						}
					}

					if (bInvert)
						Algo::Reverse(AddedVertices);

					//
					// Let's start making faces with the hole
					TArray<FVector> Left, Right;
					FVector PreviousSegment = AddedVertices[0];
					// split the array in two and build
					for (int idx = 1; idx < FGenericPlatformMath::CeilToInt(AddedVertices.Num() / 2.f) - 1; idx++)
					{
						Left.Add(AddedVertices[idx]);
						Right.Add(AddedVertices.Last(idx));
						// this needs to be fixed
	                    Volume += SignedVolumeOfTriangle(	ScaleTransform.TransformPosition(Left[idx - 1]), // previous Left
														    ScaleTransform.TransformPosition(Right[idx - 1]), // current right
														    ScaleTransform.TransformPosition(Right[idx - 2])); // previous Right
					}
				}
			}
		}
		return Volume;
	}
#endif // WITH_PHYSX
	
public:
	
	/** 
	 *	GetConvexTruncatedVolume Calculate volume of a Convex element (of a body setup for example) when cut by a plane  
	 */
	static float GetConvexTruncatedVolume(const FKConvexElem &ConvexElement, const FVector &PlaneRelativePosition, const FVector &PlaneNormal, const FVector& Scale)
	{	
	#if WITH_PHYSX
		auto pxConvex = ConvexElement.GetConvexMesh();
		return GetPhysXConvexTruncatedVolume(pxConvex, PlaneRelativePosition, PlaneNormal, Scale);
	#endif // WITH_PHYSX
		return 0.f;
	}

	/** 
	 *	GetSphylTruncatedVolume Calculate volume of a sphyl element (of a body setup for example) when cut by a plane
	 *	@see https://en.wikipedia.org/wiki/Spherical_cap
	 */
	static float GetSphereTruncatedVolume(const FKSphereElem &SphereElement, const FVector &PlaneRelativePosition, const FVector &PlaneNormal, const FVector& Scale)
	{
		float Volume = 0.f;


		float Radius = SphereElement.Radius * Scale.GetMin();
		
		//we first need to se ethe sphere in the Plane local space
		const FVector NormalizedPlaneNormal =  PlaneNormal.GetSafeNormal();
		FRotator PlaneOrientation =  UKismetMathLibrary::MakeRotFromZ(NormalizedPlaneNormal);
		// get the plane transform
		const FTransform PlaneTrans = FTransform(PlaneOrientation, PlaneRelativePosition, FVector::OneVector);
		
		// get the sphere location compared to the plane
		const FVector RelPos = PlaneTrans.InverseTransformPosition(SphereElement.Center);

		// completely over 
		if(RelPos.Z >= Radius )
			return Volume;

		// completely under
		if(RelPos.Z <= Radius )
			return SphereElement.GetVolume(Scale);
		
		const float height = (Radius - RelPos.Z);
		// Volume of a sphere  :  {  4 / 3 * PI * FMath::Pow(Radius * Scale.GetMin(), 3); }
		// Volume of a truncated sphere = (pi * h^2 /3) (3 * r - h)
		Volume =  (PI * FMath::Pow(height, 2) / 3) * ((3 * Radius) - height) ;

		return Volume;	
	}
	
	/** 
	 *	GetSphylTruncatedVolume Calculate volume of a sphyl element (of a body setup for example) when cut by a plane  
	 */
	static float GetSphylTruncatedVolume(const FKSphylElem &SphylElement, const FVector &PlaneRelativePosition, const FVector &PlaneNormal, const FVector& Scale)
	{
		UE_LOG(LogNAVIS_Physics, Error, TEXT("Sphyl not implemented"));
		return 0.f;
	}

	/** 
	 *	GetTaperedCapsuleTruncatedVolume Calculate volume of a tapered Capsule  element (of a body setup for example) when cut by a plane  
	 */
	static float GetTaperedCapsuleTruncatedVolume(const FKTaperedCapsuleElem &TaperedCapsuleElement, const FVector &PlaneRelativePosition, const FVector &PlaneNormal, const FVector& Scale)
	{
		UE_LOG(LogNAVIS_Physics, Error, TEXT("TaperedCapsule not implemented"));
		return 0.f;
	}
	
	/** 
	 *	GetBoxTruncatedVolume Calculate volume of a box element (of a body setup for example) when cut by a plane  
	 *	https://math.stackexchange.com/a/455711
	 */
	static float GetBoxTruncatedVolume(const FKBoxElem &BoxElement, const FVector &PlaneRelativePosition, const FVector &PlaneNormal, const FVector& Scale)
	{
		UE_LOG(LogNAVIS_Physics, Error, TEXT("box not implemented"));

		// first find a scale that turn the cuboid into a real cube :
		FVector BoxScale = FVector(BoxElement.X, BoxElement.Y, BoxElement.Z);

		// we consider the box in 0.0; with extend only in the positive 
		FVector BoxDimension = FVector(BoxElement.X * 2, BoxElement.Y * 2, BoxElement.Z * 2);
		FVector PlaneBoxPos = PlaneRelativePosition - BoxScale;
		//const float Alpha = 

		// now we can work in a 1:1:1 scale for the plane 
		const FTransform PlaneTransform = FTransform(UKismetMathLibrary::MakeRotFromZ(NormalizedPlaneNormal), PlaneBoxPos,  BoxScale);
		float Volume = 0.f;
	
		return Volume;
	}

	/** 
	 *	GetPhysicsTruncatedVolume Calculate volume of a Physx element (of a body instance most likely) when cut by a plane  
	 */
	static float GetPhysicsTruncatedVolume(FPhysicsShapeHandle &PhysXElement, const FVector &PlaneRelativePosition, const FVector &PlaneNormal, const FVector& Scale )
	{
		float Volume = 0.f;
	#if WITH_PHYSX

		if(!PhysXElement.IsValid())
			return Volume;

		const auto Type = PhysXElement.Shape->getGeometryType();
		switch(Type)
		{
			case physx::PxGeometryType::eCAPSULE 		:
			UE_LOG(LogNAVIS_Physics, Error, TEXT("GetPhysxTruncatedVolume : PhysX PxCapsuleGeometry direct calculation not implemented"));
			{
				physx::PxCapsuleGeometry Capsule;
				PhysXElement.Shape->getCapsuleGeometry(Capsule);
			}
			break;
			case physx::PxGeometryType::eBOX	 		:
			UE_LOG(LogNAVIS_Physics, Error, TEXT("GetPhysxTruncatedVolume : PhysX PxBoxGeometry direct calculation not implemented"));
			{
				physx::PxBoxGeometry Box;
				PhysXElement.Shape->getBoxGeometry(Box);
			}
			break;
			case physx::PxGeometryType::eCONVEXMESH	:
			{
				physx::PxConvexMeshGeometry Convex;
				PhysXElement.Shape->getConvexMeshGeometry(Convex);
				if(Convex.isValid() && Convex.convexMesh)
				{
					Volume = GetPhysXConvexTruncatedVolume(Convex.convexMesh, PlaneRelativePosition, PlaneNormal, Scale);
				}
			}
			break;
			case physx::PxGeometryType::eSPHERE			:	
			UE_LOG(LogNAVIS_Physics, Error, TEXT("GetPhysxTruncatedVolume : PhysX PxSphereGeometry direct calculation not implemented"));
			{
				physx::PxSphereGeometry Sphere;
				PhysXElement.Shape->getSphereGeometry(Sphere);
			}
			break;
			case physx::PxGeometryType::eHEIGHTFIELD	:
			case physx::PxGeometryType::ePLANE			:
			case physx::PxGeometryType::eTRIANGLEMESH	:
			default :
			UE_LOG(LogNAVIS_Physics, Error, TEXT("GetPhysxTruncatedVolume : PhysX Extra cases direct calculation not implemented"));
			break;
		}
	#endif // WITH_PHYSX
	return Volume;
	}
};
