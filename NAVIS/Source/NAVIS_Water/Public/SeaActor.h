// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#pragma once

#include "GameFramework/Actor.h"
#include "SeaActor.generated.h"

class USeaSurfaceComponent;
class UBoxComponent;
class UPostProcessComponent;

/** 
 *	@class ASeaActor 
 *  Actor representing the sea. handles visual as well as collisions 
 */
UCLASS(Category = "WATER", hideCategories = ("Input", "Actor Tick", "Replication"))
class NAVIS_WATER_API ASeaActor : public AActor
{
    GENERATED_BODY()

public:

    /** ASeaActor   constructor  */
    ASeaActor();

    //~ Begin AActor Interface.
    virtual void BeginPlay() override;
    //~ End AActor Interface.


protected: 
    /** Extent    How spread this water is  */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter = "ApplyExtent" )
    FVector2D Extent;

public:

    /**
	 * 	ApplyExtent()		        Change the Extent of the Water Area
	 * 	@param newExtent			the new extent you want to apply
     *  @note                       called when Extent is changed in blueprint, but you have to call it   
	 */
    UFUNCTION(BlueprintSetter, BlueprintCallable)
    void ApplyExtent(const FVector2D &newExtent);


protected:


    static FName SurfaceName;       /** SurfaceName         static name for @see SurfaceComp  */
    static FName VolumeName;        /** VolumeName          static name for @see VolumeComp  */
    static FName PostProcessName;   /** PostProcessName     static name for @see PPComp     */

private:

	/** SurfaceComp    Surface and root component of the actor  */
    UPROPERTY(VisibleDefaultsOnly, meta=(AllowPrivateAccess = "true"))
    USeaSurfaceComponent * SurfaceComp;

	/** VolumeComp    Collison handler for this actor  */
    UPROPERTY()
    UBoxComponent  * VolumeComp;

    /** PPComp        Post process component, responsible for underwater effect  */
    UPROPERTY(VisibleDefaultsOnly, meta=(AllowPrivateAccess = "true"))
    UPostProcessComponent  * PPComp;

    /**
	 * 	OnEnterVolume()		        Callback called when something enters this actor
	 * 	@param overlappedComponent	Will always be VolumeComp, as this is the component associated with this callback
     *  @param otherActor	        Whatever entered the water
     *  @param otherComp	        the specific component which entered the volume
     *  @param otherBodyIndex       Not used
     *  @param bFromSweep           Not used
     *  @param sweepResult          Structure holding info on impact (normal, location, etc..)
	 */
    UFUNCTION(meta=(UnsafeDuringActorConstruction="true"))
    virtual void OnEnterVolume( UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult);

     /**
     * 	OnLeaveVolume()		        Callback called when something leaves this actor
	 * 	@param overlappedComponent	Will always be VolumeComp, as this is the component associated with this callback
     *  @param otherActor	        Whatever entered the water
     *  @param otherComp	        the specific component which entered the volume
     *  @param otherBodyIndex       Not used
	 */
    UFUNCTION(meta=(UnsafeDuringActorConstruction="true"))
    virtual void OnLeaveVolume( UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex);

    /** OverlappingActors        Array of currently overlapping actors, initialized at @see BeginPlay() and updated on @see OnEnterVolume() and OnLeaveVolume()  */
    UPROPERTY(transient)
    TArray<AActor *> OverlappingActors; 

public:

     /**
     * 	Event_OnActorEnteredVolume()    Callback called when something leaves this actor
     *  @param otherActor	            Whatever entered the water
     *  @note                           Blueprint Event for prototyping
     *  @todo                           Replace by actual code in  @see OnEnterVolume()
	 */
    UFUNCTION(BlueprintImplementableEvent)
    void Event_OnActorEnteredVolume(AActor* otherActor);

     /**
     * 	Event_OnActorLeftVolume()       Callback called when something leaves this actor
     *  @param OtherActor	            Whatever entered the water
     *  @note                           Blueprint Event for prototyping
     *  @todo                           Replace by actual code in  @see OnLeaveVolume()
	 */
    UFUNCTION(BlueprintImplementableEvent)
    void Event_OnActorLeftVolume(AActor* otherActor);

};