// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#include "FloatingObjectInterface.h"
#include "NAVISPhysicsStatics.h"

void IFloatingObjectInterface::ApplyArchimedesForce(const FPlane &liquidPlane, float density,  FName boneName)
{

    UPrimitiveComponent* FloatingObject = GetFloatingComponent();

    if(!FloatingObject)
        return;

    FVector ArchimedeForce = UNAVISPhysicsStatics::GetArchimedesForce(FloatingObject,FLiquidSurface(liquidPlane, density));
    FloatingObject->AddForce(ArchimedeForce, boneName, false);
}
