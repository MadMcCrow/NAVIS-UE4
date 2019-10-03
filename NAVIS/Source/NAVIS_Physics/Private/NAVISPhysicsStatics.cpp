// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#include "NAVISPhysicsStatics.h"
#include "NAVIS_PhysicsPCH.h"
#include "NAVISVolumeMath.h"
#include "Engine/World.h"


FVector UNAVISPhysicsStatics::GetGravityDirectionAndStrength(const UObject* WorldContextObject)
{
	if(!WorldContextObject || !WorldContextObject->GetWorld())
		return FVector::ZeroVector;
	
	return FVector(0.f, 0.f, WorldContextObject->GetWorld()->GetGravityZ());
}

UPrimitiveComponent * UNAVISPhysicsStatics::GetActorPrimitive(const AActor *in)
{
	if(!in)
		return nullptr;

	UPrimitiveComponent * Component = Cast<UPrimitiveComponent>(in->GetRootComponent());
	
	if(!Component)
		Component = in->FindComponentByClass<UPrimitiveComponent>();

	return Component;
}

float UNAVISPhysicsStatics::GetActorMass(const AActor *in)
{
	if(!in || !GetActorPrimitive(in))
		return 0.f;
	return GetActorPrimitive(in)->GetMass();
}

float UNAVISPhysicsStatics::GetActorVolume(const AActor * in)
{
	if(!in || !GetActorPrimitive(in))
		return 0.f;
	return GetPrimitiveVolume(GetActorPrimitive(in));
}

float UNAVISPhysicsStatics::GetPrimitiveVolume(const UPrimitiveComponent * in)
{
	if(!in)
		return 0.f;
	if(!in->BodyInstance.BodySetup.IsValid())
		return 0.f;
	return in->BodyInstance.BodySetup.Get()->GetVolume(FVector::OneVector);
}


float UNAVISPhysicsStatics::GetPrimitiveVolumeAtLevel(const UPrimitiveComponent * in, const FVector &PlaneWorldPosition, const FVector &PlaneNormal)
{
	if(!in)
		return 0.f;

	const FVector PlaneRelativePosition = in->GetComponentToWorld().TransformPosition(PlaneWorldPosition);

	if(in->BodyInstance.BodySetup.Get())
		return GetBodySetupVolumeAtLevel(in->BodyInstance.BodySetup.Get(), PlaneRelativePosition, PlaneNormal);
	
}

float UNAVISPhysicsStatics::GetBodySetupVolumeAtLevel(const UBodySetup * in, const FVector &PlaneRelativePosition, const FVector &PlaneNormal)
{

	float Volume = 0.f;
	FVector Scale = FVector::OneVector;
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


float UNAVISPhysicsStatics::GetBodyInstanceVolumeAtLevel(const FBodyInstance &in, const FVector &PlaneRelativePosition, const FVector &PlaneNormal )
{
	float Volume = 0.f;
    TArray<FPhysicsShapeHandle> Shapes;

	in.GetAllShapes_AssumesLocked(Shapes);

	for (FPhysicsShapeHandle Itr : Shapes)
	{
		Volume += FNAVISVolumeMath::GetPhysicsTruncatedVolume(Itr,PlaneRelativePosition, PlaneNormal, FVector::OneVector );
	}
	return Volume;
}


FVector GetArchimedesForce(const AActor *in)
{
	return FVector::ZeroVector;
}

