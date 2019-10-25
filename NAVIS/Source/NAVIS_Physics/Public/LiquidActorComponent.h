#pragma once

#include "Components/ActorComponent.h"
#include "NAVISPlane.h"
#include "LiquidActorComponent.generated.h"


/**
 *  NAVIS_PHYSICS - minimalAPI
 *  ULiquidActorComponent
 *	UActorComonent class that handle waves, forces call etc...
 */
UCLASS(minimalAPI)
class ULiquidActorComponent : public UActorComponent
{
    GENERATED_BODY()
public:

	/**
	 * Default UObject constructor that takes an optional ObjectInitializer.
	 */
	ULiquidActorComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  
	/**
	 * 	GetSurfaceNormal()			Gets the global surface normal unaffected by surface variations (ie Waves)
	 */
    virtual FVector GetSurfaceNormal() const;

	/**
	 * 	GetLocalSurfaceNormal()			Gets the surface normal affected by surface variations (ie Waves)
	 */
    virtual FVector GetLocalSurfaceNormal() const;
    
    /**
	 * 	GetSurfaceLocationUnderPoint()	Calculate Force applied to a component when put in water
	 *	@param traceOrigin  			Position of a point above (or inside) liquid that you want to project onto the liquid surface
	 * 	@param surfaceNormal			the normal you use for projection, defaults to GetSurfaceNormal();
	 */
    virtual FVector GetSurfaceLocationUnderPoint(FVector traceOrigin) const;
    

private:

	UPROPERTY()
    FLiquidSurface LiquidSurface;

};