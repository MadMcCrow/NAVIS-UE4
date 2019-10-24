// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FloatingObjectInterface.generated.h"

/**
 *  NAVIS_PHYSICS
 *  UFloatingObjectInterface
 *	Interface class to add ready made functions for floating Actors (or their components) 
 */
UINTERFACE()
class NAVIS_PHYSICS_API UFloatingObjectInterface : public UInterface
{
	//GENERATED_UINTERFACE_BODY()
    GENERATED_BODY()
};

class NAVIS_PHYSICS_API IFloatingObjectInterface
{
	//GENERATED_IINTERFACE_BODY()
	GENERATED_BODY()

public:
	/**
	 * 	ApplyArchimedesForce()			Calculate Force applied to a component when put in water
	 *	@param liquidPlane  			Plane of water in world space
	 * 	@param density				    The density of the liquid
     *  @param boneName                 Name of the bone to wich apply force, defaults to root
	 *	@note                           We don't use a FNavisPlane to avoid unnecessary import
	 *                                  from other modules even thought we added FNavisPlane to the
	 */
	UFUNCTION()
	virtual void ApplyArchimedesForce(const FPlane &liquidPlane, float density = 1.f, FName boneName = NAME_None);

	UFUNCTION()
	virtual UPrimitiveComponent* GetFloatingComponent() const;

};