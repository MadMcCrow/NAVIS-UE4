// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#include "FloatingObjectInterface.h"
#include "NAVISPhysicsStatics.h"

void IFloatingObjectInterface::ApplyArchimedesForce(const FPlane &liquidPlane, float density = 1.f,  FName BoneName = NAME_None)
{

    UPrimitiveComponent* FloatingObject = GetFloatingComponent();

    if(!FloatingObject)
        return;

    FVector ArchimedeForce = UNAVISPhysicsStatics::GetArchimedesForce(FloatingObject,FLiquidSurface(liquidPlane, density) )
    FloatingObject->AddForce(ArchimedeForce
    FName BoneName,
    bool bAccelChange
)


}

UPrimitiveComponent* IFloatingObjectInterface::GetFloatingComponent() const
{
    return nullptr;
}