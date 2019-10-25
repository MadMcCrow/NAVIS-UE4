// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NAVISPlane.generated.h"

// forward declarations
class UBodySetup;
class AActor;

/**
 *  NAVIS_Types - MinimalAPI
 *  FNavisPlane
 *	Normalized plane structure based on FPlane
 */
USTRUCT()
struct NAVIS_TYPES_API FNavisPlane : public FPlane
{
	GENERATED_BODY()

public:

	/**
	 * Constructor
	 * @note Override all constructors to normalise after constructor
	 */
	FNavisPlane() :FPlane() {};
	FORCEINLINE FNavisPlane(const FVector4& V) : FPlane(V){	SetNormalToUnit();	}
	FORCEINLINE FNavisPlane(float InX, float InY, float InZ, float InW): FPlane(InX, InY,  InZ, InW) {	SetNormalToUnit();	}
	FORCEINLINE FNavisPlane(FVector InNormal, float InW): FPlane( InNormal,  InW) {	SetNormalToUnit();	}
	FORCEINLINE FNavisPlane(FVector InBase, const FVector &InNormal): FPlane( InBase,  InNormal) {	SetNormalToUnit();	}
	FORCEINLINE FNavisPlane(FVector A, FVector B, FVector C) : FPlane( A,  B,  C) {	SetNormalToUnit();	}

	/**	
	 * GetNormal()	return normal of length 1
	 */
	FORCEINLINE FVector GetNormal() const
	{
		const float SquareSum = X*X + Y*Y + Z*Z;
		if(SquareSum != 0.f && SquareSum != 1.f)
		{
			const float scale = FMath::InvSqrt(SquareSum);
			return FVector(X* scale, Y* scale, Z*scale);
		}
		return FVector(X, Y, Z);
	}

	/**	
	 * SetNormalToUnit()	make sure the normal of the plane stored is of length 1, make sure that the W parameter is affected to
	 */
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

	/**	
	 * GetClosestPoint()	@return closest point to plane (in the same reference transform)
	 */
	FORCEINLINE FVector	GetClosestPoint(const FVector location) const
	{
		return FVector::PointPlaneProject(location, *this);
	}

	/**	
	 * GetLocalPosition()	@return Plane position transformed from World To Local
	 */
    FORCEINLINE FVector GetLocalPosition(const FTransform &localToWorld ) const
    {
        return localToWorld.TransformPosition(GetPosition());
    }

	/**	
	 * GetLocalNormal()		@return Plane normal transformed from World To Local
	 */
    FORCEINLINE FVector GetLocalNormal(const FTransform &localToWorld ) const
    {
        return localToWorld.TransformVector(GetNormal());
    }

	/**	
	 * GetPlaneOrigin()		Get a position on the plane, the one used in making this plane
	 */
	FORCEINLINE FVector GetPlaneOrigin() const
	{
		//SetNormalToUnit(); // might not be necessary
		return FVector( X, Y, Z ) * W; // Verify this
	}

	/**	
	 * GetPosition()	get a position on the plane
	 * @todo 			should be changed, as it doubles with @see GetPlaneOrigin()
	 */
	virtual FVector	GetPosition() const
	{
		return GetPlaneOrigin();
	}
};


/**
 *  NAVIS_TYPES
 *  FLiquidSurface
 *	Normalized plane with extra liquid oriented data.
 */
USTRUCT()
struct NAVIS_TYPES_API FLiquidSurface : public FNavisPlane
{
	GENERATED_BODY()
protected:

	/**	
	 * Density 		density of the liquid. Pure water has a value of 1
	 * @note 		defaults to one via constructor
	 */
	UPROPERTY(BlueprintReadOnly)
	float Density;

public:
	
	/**	
	 * GetDensity()	Get @see Density, via copy
	 */
	virtual float GetDensity() const { return Density; }

	FORCEINLINE FLiquidSurface() : FNavisPlane(), Density(1.f) {} // default constructor, necessary
	FORCEINLINE FLiquidSurface(const FNavisPlane &in, float density = 1.f) : FNavisPlane(in), Density(density){}
	FORCEINLINE FLiquidSurface(const FPlane &in, float density = 1.f) : FNavisPlane(in), Density(density)	{}
	FORCEINLINE FLiquidSurface(FVector InNormal, float InW, float density = 1.f): FNavisPlane( InNormal,  InW), Density(density) {}
	FORCEINLINE FLiquidSurface(FVector InBase, const FVector &InNormal, float density = 1.f): FNavisPlane( InBase,  InNormal), Density(density) {}
	FORCEINLINE FLiquidSurface(FVector A, FVector B, FVector C, float density = 1.f) : FNavisPlane( A,  B,  C), Density(density) {}
	FORCEINLINE FLiquidSurface(float InX, float InY, float InZ, float InW, float density = 1.f): FNavisPlane(InX, InY,  InZ, InW), Density(density) {}
};