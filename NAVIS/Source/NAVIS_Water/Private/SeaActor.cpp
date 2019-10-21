// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#include "SeaActor.h"
#include "SeaSurfaceComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PostProcessComponent.h"

FName ASeaActor::SurfaceName        = FName("SeaComp");
FName ASeaActor::VolumeName         = FName("UnderWaterComp");
FName ASeaActor::PostProcessName    = FName("EffectComp");

ASeaActor::ASeaActor() : Super() , Extent(FVector2D(100.f, 100.f))
{
    SurfaceComp = CreateDefaultSubobject<USeaSurfaceComponent>(SurfaceName);
    VolumeComp  = CreateDefaultSubobject<UBoxComponent>(VolumeName);
    PPComp      = CreateDefaultSubobject<UPostProcessComponent>(PostProcessName);

    RootComponent = SurfaceComp;
    VolumeComp->SetupAttachment(SurfaceComp);
    PPComp->SetupAttachment(VolumeComp);

    VolumeComp->OnComponentBeginOverlap.AddDynamic(this, &ASeaActor::OnEnterVolume);
    VolumeComp->OnComponentEndOverlap.AddDynamic(this,  &ASeaActor::OnLeaveVolume);

    ApplyExtent(Extent);
}

 void ASeaActor::BeginPlay()
 {
    Super::BeginPlay();

    if(VolumeComp)   // May be unecessary
    {
        VolumeComp->GetOverlappingActors(OverlappingActors, /*TSubclassOf<AActor> ClassFilter*/ nullptr);
    }   
 }

void ASeaActor::ApplyExtent(const FVector2D &newExtent)
{
    Extent = newExtent;
    /** @todo  Check if scale or unscaled should be called */
    const float depth = VolumeComp->GetUnscaledBoxExtent().Z;
    const FVector Extent3d = FVector(Extent,depth);

    VolumeComp->SetBoxExtent(Extent3d, true);
    VolumeComp->SetRelativeLocation(FVector(0.f,0.f, -1.f * depth));

	//FVector Origin = SurfaceComp->Bounds.Origin;
	const FVector	BoxExtent = SurfaceComp->Bounds.BoxExtent;
    SurfaceComp->SetRelativeScale3D(Extent3d / BoxExtent);
}

void ASeaActor::OnEnterVolume( UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult)
{
    if(!OverlappingActors.Contains(otherActor)) // may be unecessary
        OverlappingActors.AddUnique(otherActor);

    // notify BP
    Event_OnActorEnteredVolume(otherActor);
}

void ASeaActor::OnLeaveVolume( UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
    if(OverlappingActors.Contains(otherActor)) // may be unecessary
        OverlappingActors.Remove(otherActor);
    
    // notify BP
    Event_OnActorLeftVolume(otherActor);
}