// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "NAVISPhysicsStatics.generated.h"

class AActor;

USTRUCT()
struct NAVIS_PHYSICS_API FNavisPlane : public FPlane
{
	GENERATED_BODY()
public:

	//	Override all constructors to normalise after constructor
	FNavisPlane() :FPlane() {};

	FORCEINLINE FNavisPlane(const FVector4& V) : FPlane(V){	SetNormalToUnit();	}
	FORCEINLINE FNavisPlane(float InX, float InY, float InZ, float InW): FPlane(InX, InY,  InZ, InW) {	SetNormalToUnit();	}
	FORCEINLINE FNavisPlane(FVector InNormal, float InW): FPlane( InNormal,  InW) {	SetNormalToUnit();	}
	FORCEINLINE FNavisPlane(FVector InBase, const FVector &InNormal): FPlane( InBase,  InNormal) {	SetNormalToUnit();	}
	FORCEINLINE FNavisPlane(FVector A, FVector B, FVector C) : FPlane( A,  B,  C) {	SetNormalToUnit();	}

	FORCEINLINE FVector GetNormal() const
	{
		const float SquareSum = X*X + Y*Y + Z*Z;
		if(SquareSum != 0.f && SquareSum != 1.f)
		{
			const float scale = FMath::InvSqrt(SquareSum);
			return FVector(X*SquareSum, Y*SquareSum, Z*SquareSum);
		}
		return FVector(X, Y, Z);
	}
	
	void SetNormalToUnit()
	{
		// doing it UE4 style ;)
		const float SquareSum = X*X + Y*Y + Z*Z;
		if (SquareSum != 0.f && SquareSum != 1.f)
		{
			const float scale = FMath::InvSqrt(SquareSum);
			X = X/scale;
			Y = Y/scale;
			Z = Z/scale;
			W = W/scale;
		}
	}
    
	FORCEINLINE FVector	GetClosestPoint(const FVector location) const
	{
		return FVector::PointPlaneProject(location, *this);
	} 

    FORCEINLINE FVector GetLocalPosition(const FTransform &localToWorld ) const
    {
        return localToWorld.TransformPosition(GetPosition());
    }

    FORCEINLINE FVector GetLocalNormal(const FTransform &localToWorld ) const
    {
        return localToWorld.TransformVector(GetNormal());
    }


	FORCEINLINE FVector GetPlaneOrigin() const 
	{
		//SetNormalToUnit(); // might not be necessary
		return FVector( X, Y, Z ) * W; // Verify this
	}

	// 	This should be removed
	FVector	GetPosition() const
	{
		return GetPlaneOrigin();
	} 
};

USTRUCT() 
struct NAVIS_PHYSICS_API FLiquidSurface : public FNavisPlane
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	float Density;

public:

	float GetDensity() const { return Density; }

};

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
	static float GetPrimitiveVolumeAtLevel(const UPrimitiveComponent *in, const FNavisPlane &worldPlane);

	/**
	 * 	GetBodySetupVolumeAtLevel()		Calculate Volume for a body setup when cut by a plane (like a sea level)
	 * 	@param in						The body setup to consider.
 	 *	@param relativePlane			Plane made of a position of a point of the plane in relative space and its normal
	 */
	UFUNCTION()
	static float GetBodySetupVolumeAtLevel(const UBodySetup *in, const FNavisPlane &relativePlane);

	/**
	 * 	GetBodyInstanceVolumeAtLevel()	Calculate Volume for a body setup when cut by a plane (like a sea level)
	 * 	@param in						The body Instance to consider.
 	 *	@param relativePlane			Plane made of a position of a point of the plane in relative space and its normal
	 */
	UFUNCTION()
	static float GetBodyInstanceVolumeAtLevel(const FBodyInstance &in, const FNavisPlane &relativePlane);

	/**
	 * 	GetArchimedesForce()			Calculate Force applied to an actor when put in water
	 * 	@param in						The Actor in Water
	 *	@param WorldPlane				Plane of water in world space
	 *	@return 						A Force in Newton stored in a world vector
	 */
	UFUNCTION(BlueprintPure, Category = "Force")
	static FVector GetArchimedesForce(const AActor *in, const FLiquidSurface &liquidWorldPlane);
};
