// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#pragma once

#include "Components/StaticMeshComponent.h"
#include "SeaSurfaceComponent.generated.h"


class UCanvasRenderTarget2D;

/** 
 *	@class USeaSurfaceComponent 
 *  component responsible for making the water look like real water
 */
UCLASS(minimalAPI, Category = "WATER", hideCategories = ("Variable", "Transform", "Sockets", "Physics", "Collision", "Component Replication", "Events"))
class USeaSurfaceComponent : public UStaticMeshComponent
{
    GENERATED_BODY()

public:
    USeaSurfaceComponent();

	//~ Begin UActorComponent Interface.
    virtual void BeginPlay() override;
    //~ End UActorComponent Interface.

    /**
     * 	Event_OnActorLeftVolume()       Callback called when something leaves this actor
     *  @param worldLocation	        Whatever entered the water
     *  @note                           Blueprint Event for prototyping
	 */
    virtual FVector WorldToLocalScaledLocation(const FVector &worldLocation) const;


protected:

    /**
     *  GetRenderTarget()       Render Target used to produce object on water effects  
     *  @returns                RenderTarget, will create it if not already present
     */
    virtual UCanvasRenderTarget2D * GetRenderTarget();

    /**
     *  RenderTargetDimension   How precise our render target shall be
     *  @note   	           We might want to make this non static to be allowed to change resolution dynamically
     */
    static struct FRenderTargetDimension 
    {
        uint16 X;
        uint16 Y; 
        FRenderTargetDimension(uint16 newX, uint16 newY ) : X(newX), Y(newY) {}
    }RenderTargetDimension; //: uint32

private :

    /**
     *  RenderTarget    Render Target used to produce object on water effects  
     *  @note           UTextureRenderTarget2D may be enougth, even UTextureRenderTarget
     */
    UPROPERTY()
    UCanvasRenderTarget2D * RenderTarget;
};