// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#include "NAVISPhysicsStatics.h"
#include "NAVIS_PhysicsPCH.h"
#include "NAVISVolumeMath.h"



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


static float UNAVISPhysicsStatics::GetPrimitiveVolumeAtLevel(const UPrimitiveComponent * in, const FVector &PlaneWorldPosition, const FVector &PlaneNormal = FVector::UpVector )
{
	

	if(!in)
		return 0.f;

	const FVector PlaneRelativePosition = in->GetComponentToWorld().TransformPosition(PlaneWorldPosition);

	if(in->BodyInstance.BodySetup.Get())
		return GetBodySetupVolumeAtLevel(in->BodyInstance.BodySetup.Get(), PlaneRelativePosition, PlaneNormal);
	
}

static float UNAVISPhysicsStatics::GetBodySetupVolumeAtLevel(const UBodySetup * in, const FVector &PlaneRelativePosition, const FVector &PlaneNormal = FVector::UpVector )
{

	float Volume = 0.f;
	// Sphere			:
	for (auto itr : in->AggGeom.SphereElems)
		Volume += FNAVISVolumeMath::GetSphereTruncatedVolume(itr, PlaneRelativePosition, PlaneNormal, Scale);
	// Box				:
	for (auto itr : in->AggGeom.SphereElems)
		Volume += FNAVISVolumeMath::GetBoxTruncatedVolume(itr, PlaneRelativePosition, PlaneNormal, Scale);
	// Sphyl			:
	for (auto itr : in->AggGeom.SphereElems)
		Volume += FNAVISVolumeMath::GetSphylTruncatedVolume(itr, PlaneRelativePosition, PlaneNormal, Scale);
	// Convex			:
	for (auto itr : in->AggGeom.ConvexElems)
		Volume += FNAVISVolumeMath::GetConvexTruncatedVolume(itr, PlaneRelativePosition, PlaneNormal, Scale);
	// TaperedCapsule	:
	for (auto itr : in->AggGeom.SphereElems)
		Volume += FNAVISVolumeMath::GetTaperedCapsuleTruncatedVolume(itr, PlaneRelativePosition, PlaneNormal, Scale);

	// Unknown ?

	return Volume;
}



