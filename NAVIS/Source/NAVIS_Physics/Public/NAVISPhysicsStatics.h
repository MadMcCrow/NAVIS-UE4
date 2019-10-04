// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "NAVISPhysicsStatics.generated.h"

class AActor;

USTRUCT()
struct NAVIS_PHYSICS_API FPlane
{
protected:
	UPROPERTY()
	FVector Position;

	UPROPERTY()
	FVector Normal;

public:
	FPlane(const FVector &planePosition = FVector::ZeroVector, const FVector &planeNormal = FVector::UpVector)
	{
		Position = planePosition;
		Normal = planeNormal;
	}

	FVector GetPosition() const { return Position; }
	FVector GetNormal() const { return Normal; }
}

USTRUCT()
struct NAVIS_PHYSICS_API FLiquidSurface : public FPlane
{
protected:
	UPROPERTY()
	float Density;

public:

	float GetDensity() const { return Density; }	

}

/**
 * Function used in various physics calculations
 */
UCLASS(Category = "Physics|Statics") class NAVIS_PHYSICS_API UNAVISPhysicsStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template <typename T>
	constexpr static T UnrealToMeter(T inUE)
	{
		return inUE * 0.01f;
	}

	template <typename T>
	constexpr static T MeterToUnreal(T inMeter)
	{
		return inMeter * 100.f;
	}

	/**
	 * 	floatUnrealToMeter()		Convert a measure of distance in Unreal value to meter
	 * 	@param UESize				your value
	 */
	UFUNCTION(BlueprintPure, Category = "Units", meta = (DisplayName = "UE4 Size To Meter"))
	static float floatUnrealToMeter(float UESize) { return UnrealToMeter<float>(UESize); }

	/**
	 * 	floatMeterToUnreal()		Convert a measure of distance in meter to Unreal value
	 * 	@param meter				your value
	 */
	UFUNCTION(BlueprintPure, Category = "Units", meta = (DisplayName = "Meters To UE4 size"))
	static float floatMeterToUnreal(float meter) { return MeterToUnreal<float>(meter); }

	/**
	 * 	GetGravityDirectionAndStrength()		gets the current gravity set in level
	 * 	@param WorldContextObject				valid object in a valid world context
	 */
	UFUNCTION(BlueprintPure, Category = "World", meta = (WorldContext = "WorldContextObject"))
	static FVector GetGravityDirectionAndStrength(const UObject *WorldContextObject);

	/**
	 * 	GetActorPrimitive()		Get the first primitive component used in this actor 
	 * 	@param in				an owner containing at least one primitive component
	 *	@return					A valid component or nullptr if none was found
	 */
	UFUNCTION(BlueprintPure, Category = "Actor")
	static UPrimitiveComponent *GetActorPrimitive(const AActor *in);

	/**
	 * 	GetActorMass()					Try to get actor mass as described in UE4
	 * 	@param in						The massed actor
	 */
	UFUNCTION(BlueprintPure, Category = "Actor")
	static float GetActorMass(const AActor *in);

	/**
	 * 	GetActorVolume()		Calculate Volume of an actor
	 * 	@param in				an owner containing at least one primitive component
	 */
	UFUNCTION(BlueprintPure, Category = "Volume")
	static float GetActorVolume(const AActor *in);

	/**
	 * 	GetPrimitiveVolume()		Calculate Volume of a component
	 * 	@param in					the primitive component (mostly static mesh and skeletal meshes components) to consider for the measure
	 */
	UFUNCTION(BlueprintPure, Category = "Volume")
	static float GetPrimitiveVolume(const UPrimitiveComponent *in);

	/**
	 * 	GetBodySetupVolumeAtLevel()		Calculate Volume for a component when cut by a plane (like a sea level)
	 * 	@param in						the primitive component (mostly static mesh and skeletal meshes components) to consider for the measure
	 *	@param worldPlane				Plane made of a position of a point of the plane in world space and its normal
	 */
	UFUNCTION(BlueprintPure, Category = "Volume")
	static float GetPrimitiveVolumeAtLevel(const UPrimitiveComponent *in, const FPlane &worldPlane);

	/**
	 * 	GetBodySetupVolumeAtLevel()		Calculate Volume for a body setup when cut by a plane (like a sea level)
	 * 	@param in						The body setup to consider.
 	 *	@param relativePlane			Plane made of a position of a point of the plane in relative space and its normal
	 */
	UFUNCTION()
	static float GetBodySetupVolumeAtLevel(const UBodySetup *in, const FPlane &relativePlane);

	/**
	 * 	GetBodyInstanceVolumeAtLevel()	Calculate Volume for a body setup when cut by a plane (like a sea level)
	 * 	@param in						The body Instance to consider.
 	 *	@param relativePlane			Plane made of a position of a point of the plane in relative space and its normal
	 */
	UFUNCTION()
	static float GetBodyInstanceVolumeAtLevel(const FBodyInstance &in, const FPlane &relativePlane);

	/**
	 * 	GetArchimedesForce()			Calculate Force applied to an actor when put in water
	 * 	@param in						The Actor in Water
	 *	@param PlaneRelativePosition	Position of a point of the plane in world space, defaulted to world origin
	 *	@param PlaneNormal				Normal of the cutting plane. since the normal of water is the inverse of gravity, we default to Up (0,0,1)
	 *	@return 						A Force in Newton stored in a world vector 
	 */
	UFUNCTION(BlueprintPure, Category = "Force")
	static FVector GetArchimedesForce(const AActor *in, const FPlane &relativePlane);
};
