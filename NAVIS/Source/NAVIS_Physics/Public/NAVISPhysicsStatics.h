// Noe Perard-Gayot <noe.perard@gmail.com> 2019 - All Rights Reserved

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "NAVISPhysicsStatics.generated.h"

class AActor;

/**
 * Function used in various physics calculations
 */
UCLASS(Category = "Physics|Statics")
class UNAVISPhysicsStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	template<typename T>
	constexpr static T UnrealToMeter(T inUE)
	{
		return inUE * 0.01f;
	}

	template<typename T>
	constexpr static T MeterToUnreal(T inMeter)
	{
		return inMeter * 100.f;
	}

	UFUNCTION(BlueprintPure, Category= "Units",  meta = (DisplayName = "UE4 Size To Meter"))
		static float floatUnrealToMeter_BP(float UESize) { return UnrealToMeter<float>(UESize); }

	UFUNCTION(BlueprintPure, Category= "Units", meta = (DisplayName = "Meters To UE4 size"))
		static float floatMeterToUnreal(float meter) { return MeterToUnreal<float>(meter); }

	UFUNCTION(BlueprintPure, Category = "Measures")
		static float GetActorVolume(const AActor * in);

	UFUNCTION(BlueprintPure, Category = "Measures")
		static float GetPrimitiveVolume(const UPrimitiveComponent * in);  

	UFUNCTION(BlueprintPure, Category = "Measures")
		static float GetPrimitiveVolumeAtLevel(const UPrimitiveComponent * in, const FVector &PlaneWorldPosition, const FVector &PlaneNormal = FVector::UpVector );

	UFUNCTION()
		static float GetBodySetupVolumeAtLevel(const UBodySetup * in, const FVector &PlaneRelativePosition, const FVector &PlaneNormal = FVector::UpVector );


};
