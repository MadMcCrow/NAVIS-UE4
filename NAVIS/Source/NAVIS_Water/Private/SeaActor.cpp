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

void ASeaActor::ApplyExtent(const FVector2D &newExtent)
{
    Extent = newExtent;
    /** @todo  Check if scale or unscaled should be called */
    const float depth = VolumeComp->GetUnscaledBoxExtent().Z;
    const FVector Extent3d = FVector(Extent,depth);

    VolumeComp->SetBoxExtent(Extent3d, true);
    VolumeComp->SetRelativeLocation(FVector(0.f,0.f, -1.f * depth));

	//FVector Origin = SurfaceComp->Bounds.Origin;
	FVector	BoxExtent = SurfaceComp->Bounds.BoxExtent;
    SurfaceComp->SetRelativeScale3D(Extent3d / BoxExtent);
}

void ASeaActor::OnEnterVolume( UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult & sweepResult)
{
    Event_OnActorEnteredVolume(OtherActor);
}

void ASeaActor::OnLeaveVolume( UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
    Event_OnActorLeftVolume(OtherActor);
}