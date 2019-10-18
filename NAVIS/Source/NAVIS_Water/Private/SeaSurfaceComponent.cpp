// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#include "SeaSurfaceComponent.h"
#include "Engine/CanvasRenderTarget2D.h"

FRenderTargetDimension USeaSurfaceComponent::RenderTargetDimension(1024,1024);

USeaSurfaceComponent::USeaSurfaceComponent() : Super()
{

}

void USeaSurfaceComponent::BeginPlay()
{
    Super::BeginPlay();
}

 FVector USeaSurfaceComponent::WorldToLocalScaledLocation(const FVector &worldLocation) const
 {
     return GetComponentTransform().InverseTransformPosition(worldLocation);
 }

UCanvasRenderTarget2D * USeaSurfaceComponent::GetRenderTarget()
{
    if(RenderTarget != nullptr)
        return RenderTarget;
    
    RenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(),RenderTargetDimension.X, RenderTargetDimension.Y );
}