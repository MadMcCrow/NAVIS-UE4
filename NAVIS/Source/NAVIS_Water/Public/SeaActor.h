// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#pragma once

#include "GameFramework/Actor.h"
#include "SeaActor.generated.h"

class USeaSurfaceComponent;

UCLASS()
class NAVIS_WATER_API ASeaActor : public AActor
{
    GENERATED_BODY()

public:
    ASeaActor();

protected:

    static FName SurfaceName;
private:
    UPROPERTY()
    USeaSurfaceComponent * SurfaceComp;

};