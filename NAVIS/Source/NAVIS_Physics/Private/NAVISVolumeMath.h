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


	static float TetrahedronVolume(const FVector& p1, const FVector& p2, const FVector& p3 ,const FVector& p4) 
	{
		return FVector::DotProduct(p1 - p4, FVector::CrossProduct(p2 - p4, p3 - p4)) / 6.0f;
	}

#if WITH_PHYSX
	// GetPhysXConvexTruncatedVolume()	works for all convex meshes as all are using physx Convex mesh
	static float GetPhysXConvexTruncatedVolume(physx::PxConvexMesh * convexMesh, const FPlane &cuttingPlane, const FVector& scale)
	{
		float Volume = -1.0f;
		
		
		if (convexMesh != NULL )
		{
			// Preparation for convex mesh scaling implemented in another changelist
			FTransform ScaleTransform = FTransform(FQuat::Identity, FVector::ZeroVector, scale);


			int32 NumPolys = convexMesh->getNbPolygons();
			PxHullPolygon PolyData;

			const PxVec3 *Vertices = convexMesh->getVertices();
			const PxU8 *Indices = convexMesh->getIndexBuffer();

			TArray<FVector>	AddedVertices;
		
			// extracting plane info
			const FVector PlaneUp = FVector(cuttingPlane.X, cuttingPlane.Y, cuttingPlane.Z);
			const float ratio = PlaneUp.Size() != 0.f ? PlaneUp.Size() : 1;
			const FVector PlaneNormal = PlaneUp / ratio;
			const FVector PlaneRelativePosition = (cuttingPlane.W / ratio) * PlaneNormal;
			const FPlane Plane = FPlane(PlaneRelativePosition, PlaneNormal);

			// finding object center on plane
			auto bounds = convexMesh->getLocalBounds();
			FVector Center = P2UVector(bounds.getCenter());
			const FVector PlaneObjectCenter = FVector::PointPlaneProject(Center,Plane); 
			
			for (int32 PolyIdx = 0; PolyIdx < NumPolys; ++PolyIdx)
			{
				if (convexMesh->getPolygonData(PolyIdx, PolyData))
				{
					for (int32 VertIdx = 2; VertIdx < PolyData.mNbVerts; ++VertIdx)
					{
						// Grab triangle indices that we hit
						int32 I0 = Indices[PolyData.mIndexBase + 0];
						int32 I1 = Indices[PolyData.mIndexBase + (VertIdx - 1)];
						int32 I2 = Indices[PolyData.mIndexBase + VertIdx];
					
						//
						// We have to determine if our points are under our plane or not
						auto IsUnderPlane = [&PlaneRelativePosition, &PlaneNormal](const FVector &Position) -> bool {
							const FVector Orient = Position - PlaneRelativePosition;
							return FVector::DotProduct(Orient, PlaneNormal) < 0;
						};

						const FVector V0 = ScaleTransform.TransformPosition(P2UVector(Vertices[I0]));
						const FVector V1 = ScaleTransform.TransformPosition(P2UVector(Vertices[I1]));
						const FVector V2 = ScaleTransform.TransformPosition(P2UVector(Vertices[I2]));
						const FVector V3 = ScaleTransform.TransformPosition(PlaneObjectCenter);
	
						const bool I0UnderPlane = IsUnderPlane(V0);
						const bool I1UnderPlane = IsUnderPlane(V1);
						const bool I2UnderPlane = IsUnderPlane(V2);

						//
						// We have four possibilities :

						//
						// Case 0 : All points are over the plane :
						if (!I0UnderPlane && !I1UnderPlane && !I2UnderPlane)
						{
							continue; // ignore those points
						}
						//
						// Case 1 : All points are below the plane :
						if (I0UnderPlane && I1UnderPlane && I2UnderPlane)
						{
							Volume += TetrahedronVolume(V0,V1,V2,V3);
							continue;
						}
						//
						// Case 2 : at least one point is under the plane :
						if(I0UnderPlane || I1UnderPlane || I2UnderPlane)
						{
							auto Intersection = [&PlaneRelativePosition, &PlaneNormal](const FVector &A, const FVector &B) -> FVector {
								const FVector Segment = B - A;
								if (Segment.Size() == 0)
									return A;
								const FVector NSegment = Segment.GetUnsafeNormal();
								const FVector PlaneToA = A - PlaneRelativePosition;
								float S = FVector::DotProduct(PlaneNormal, PlaneToA) / FVector::DotProduct(PlaneNormal, NSegment);
								return NSegment * S;
							};

							// we will always have two cuts
							FVector CutA, CutB;

							auto AddVertices = [&AddedVertices](FVector &A, FVector &B) {
								AddedVertices.AddUnique(A);
								AddedVertices.AddUnique(B);
								};

	
							if ((I0UnderPlane && (!I1UnderPlane && !I2UnderPlane)) || (I1UnderPlane && I2UnderPlane && !I0UnderPlane))
							{
								CutA = Intersection(V0, V1);
								CutB = Intersection(V0, V2);
								if (I0UnderPlane)
								{
									AddVertices(CutA, CutB);
									Volume += TetrahedronVolume(V0, CutA, CutB, V3);
								}
								else // neg alpha
								{
									AddVertices(CutB, CutA);
									Volume += TetrahedronVolume(V1, V2, CutA, V3);
									Volume += TetrahedronVolume(CutA, V2, CutB, V3);
								}
							}
							
							else if ((I1UnderPlane && (!I0UnderPlane && !I2UnderPlane)) || (I0UnderPlane && I2UnderPlane && !I1UnderPlane))
							{
								CutA = Intersection(V1, V0);
								CutB = Intersection(V1, V2);
								if (I1UnderPlane)
								{
									AddVertices(CutB, CutA);
									Volume += TetrahedronVolume(CutA, V1, CutB, V3);
								}
								else // neg beta
								{
									AddVertices(CutA, CutB);
									Volume += TetrahedronVolume(V0, CutA, V2, V3);
									Volume += TetrahedronVolume(CutA, CutB, V2, V3);
								}
							}
							else if ((I2UnderPlane && (!I0UnderPlane && !I1UnderPlane)) || (I1UnderPlane && I0UnderPlane && !I2UnderPlane))
							{
								CutA = Intersection(V2, V0);
								CutB = Intersection(V2, V1);
								if (I2UnderPlane)
								{
									AddVertices(CutA, CutB);
									Volume += TetrahedronVolume(CutA, CutB, V2, V3);
								}
								else // neg gamma
								{
									AddVertices(CutA, CutB);
									Volume += TetrahedronVolume(V0, V1, CutB, V3);
									Volume += TetrahedronVolume(V0, CutB, CutA, V3);
								}
							}
							else
							{
								UE_LOG(LogNAVIS_Physics, Error, TEXT("wrong cut, not possible"));
								Volume += TetrahedronVolume(V0,V1,V2,V3);
								continue;
							}
						}

					} // end of for (int32 VertIdx = 2; VertIdx < PolyData.mNbVerts; ++ VertIdx)
				} // if (ConvexMesh->getPolygonData(PolyIdx, PolyData))
			} // for (int32 PolyIdx = 0; PolyIdx < NumPolys; ++PolyIdx)
		} // if (ConvexMesh != NULL )
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
		return GetPhysXConvexTruncatedVolume(pxConvex, FPlane(PlaneRelativePosition, PlaneNormal), Scale);
	#endif // WITH_PHYSX
		return -1.f;
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
		const FVector NormalizedPlaneNormal = PlaneNormal.GetSafeNormal();
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
					Volume = GetPhysXConvexTruncatedVolume(Convex.convexMesh, FPlane(PlaneRelativePosition, PlaneNormal), Scale);
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
