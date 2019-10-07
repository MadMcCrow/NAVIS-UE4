// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#include "NAVISPhysicsStatics.h"
#include "NAVIS_PhysicsPCH.h"
#include "NAVISVolumeMath.h"
#include "Engine/World.h"

FVector UNAVISPhysicsStatics::GetGravityDirectionAndStrength(const UObject *WorldContextObject)
{
	if (!WorldContextObject || !WorldContextObject->GetWorld())
		return FVector::ZeroVector;

	return FVector(0.f, 0.f, WorldContextObject->GetWorld()->GetGravityZ());
}

UPrimitiveComponent *UNAVISPhysicsStatics::GetActorPrimitive(const AActor *in)
{
	if (!in)
		return nullptr;

	UPrimitiveComponent *Component = Cast<UPrimitiveComponent>(in->GetRootComponent());

	if (!Component)
		Component = in->FindComponentByClass<UPrimitiveComponent>();

	return Component;
}

float UNAVISPhysicsStatics::GetActorMass(const AActor *in)
{
	if (!in || !GetActorPrimitive(in))
		return 0.f;
	return GetActorPrimitive(in)->GetMass();
}

float UNAVISPhysicsStatics::GetActorVolume(const AActor *in)
{
	if (!in || !GetActorPrimitive(in))
		return 0.f;
	return GetPrimitiveVolume(GetActorPrimitive(in));
}

float UNAVISPhysicsStatics::GetPrimitiveVolume(const UPrimitiveComponent *in)
{
	if (!in)
		return 0.f;
	if (!in->BodyInstance.BodySetup.IsValid())
		return 0.f;
	return in->BodyInstance.BodySetup.Get()->GetVolume(FVector::OneVector);
}

float UNAVISPhysicsStatics::GetPrimitiveVolumeAtLevel(const UPrimitiveComponent *in, const FNavisPlane &worldPlane)
{
	if (!in)
		return 0.f;

	const FVector PlaneRelativePosition = in->GetComponentToWorld().TransformPosition(worldPlane.GetPosition());

	if (in->BodyInstance.BodySetup.Get())
		return GetBodySetupVolumeAtLevel(in->BodyInstance.BodySetup.Get(), FNavisPlane(PlaneRelativePosition, worldPlane.GetNormal()));
}

float UNAVISPhysicsStatics::GetBodySetupVolumeAtLevel(const UBodySetup *in, const FNavisPlane &relativePlane)
{

	float Volume = 0.f;
	FVector Scale = FVector::OneVector;
	// Sphere			:
	for (auto itr : in->AggGeom.SphereElems)
		Volume += FNAVISVolumeMath::GetSphereTruncatedVolume(itr, relativePlane.GetPosition(), relativePlane.GetNormal(), Scale);
	// Box				:
	for (auto itr : in->AggGeom.SphereElems)
		Volume += FNAVISVolumeMath::GetBoxTruncatedVolume(itr, relativePlane.GetPosition(), relativePlane.GetNormal(), Scale);
	// Sphyl			:
	for (auto itr : in->AggGeom.SphereElems)
		Volume += FNAVISVolumeMath::GetSphylTruncatedVolume(itr, relativePlane.GetPosition(), relativePlane.GetNormal(), Scale);
	// Convex			:
	for (auto itr : in->AggGeom.ConvexElems)
		Volume += FNAVISVolumeMath::GetConvexTruncatedVolume(itr, relativePlane.GetPosition(), relativePlane.GetNormal(), Scale);
	// TaperedCapsule	:
	for (auto itr : in->AggGeom.SphereElems)
		Volume += FNAVISVolumeMath::GetTaperedCapsuleTruncatedVolume(itr,  relativePlane.GetPosition(), relativePlane.GetNormal(), Scale);

	// Unknown ?

	return Volume;
}

float UNAVISPhysicsStatics::GetBodyInstanceVolumeAtLevel(const FBodyInstance &in, const FNavisPlane &relativePlane)
{
	float Volume = 0.f;
	TArray<FPhysicsShapeHandle> Shapes;

	in.GetAllShapes_AssumesLocked(Shapes);

	for (FPhysicsShapeHandle Itr : Shapes)
	{
		Volume += FNAVISVolumeMath::GetPhysicsTruncatedVolume(Itr, relativePlane.GetPosition(), relativePlane.GetNormal(), FVector::OneVector);
	}
	return Volume;
}

FVector UNAVISPhysicsStatics::GetArchimedesForce(const AActor *in, const FLiquidSurface &liquid)
{
	// We estimate the liquid to be uniform in density, In the real world, sea is not .
	const FVector direction = -1 * liquidNormal.GetSafeNormal();

	const auto solid = GetActorPrimitive(in);
	if (!solid)
		return FVector::ZeroVector;

	const  FVector liquidRelativePos =  solid->GetComponentToWorld().TransformPosition(liquid.GetPosition());
	
	const float volume = GetBodyInstanceVolumeAtLevel(solid->GetBodyInstance(), liquidRelativePos, liquid.GetNormal());
	float forceN = liquid.GetDensity() * volume;
	return forceN * direction;
}
