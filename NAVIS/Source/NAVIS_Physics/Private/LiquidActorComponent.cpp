#include "LiquidActorComponent.h"
#include "GameFramework/Actor.h"

ULiquidActorComponent::ULiquidActorComponent(const FObjectInitializer& ObjectInitializer ) :Super(ObjectInitializer)
{
    if(GetOwner())
    {
         LiquidSurface = FLiquidSurface(GetOwner()->GetActorLocation(),GetOwner()->GetActorUpVector(), 1.f);
    }
    else
    {
        LiquidSurface = FLiquidSurface(FVector::UpVector,0.f, 1.f);
    }

}

  
FVector ULiquidActorComponent::GetSurfaceNormal() const
{
    return LiquidSurface;
}

FVector ULiquidActorComponent::GetLocalSurfaceNormal() const
{
    return LiquidSurface;
}
    
FVector ULiquidActorComponent::GetSurfaceLocationUnderPoint(FVector traceOrigin ) const
{
   return LiquidSurface;
}

