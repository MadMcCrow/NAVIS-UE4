// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "NAVISPlane.h"
#include "NAVISTypesStatics.generated.h"

/**
 *  NAVIS_TYPES_API
 *  UNAVISTypesStatics
 *	Function library to use NAVIS custom types in blueprint and in other modules
 */
UCLASS(Category = "Types|Statics")
class NAVIS_TYPES_API UNAVISTypesStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * 	GetPlaneFromPointAndNormal()	get a plane from point and a non-null normal
	 * 	@param location					position of a point on the plane
	 * 	@param normal					normal of the plane
 	 *	@return							Plane according to parameters
	 */
	UFUNCTION(BlueprintPure, Category = "Data")
	static inline FNavisPlane GetPlaneFromPointAndNormal(FVector location, const FVector &normal){return FNavisPlane(location, normal);}

	/**
	 * 	GetLiquidSurface()				get a plane from point and a non-null normal
	 * 	@param plane					plane to use for making that surface
	 * 	@param density					How dense the liquid is (used for friction, forces, etc...)
 	 *	@return							FLiquidSurface according to parameters
	 */
	UFUNCTION(BlueprintPure, Category = "Data")
	static inline FLiquidSurface GetLiquidSurface(FPlane plane, float density = 1.f){return FLiquidSurface(plane, 1.f);}
	static inline FLiquidSurface GetLiquidSurface(FNavisPlane plane, float density = 1.f){return FLiquidSurface(plane, 1.f);}

};
