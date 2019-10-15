// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#include "SeaActor.h"
#include "SeaSurfaceComponent.h"

FName ASeaActor::SurfaceName = FName("SeaComp");

ASeaActor::ASeaActor() : Super()
{
    SurfaceComp = CreateDefaultSubobject<USeaSurfaceComponent>(SurfaceName);
}