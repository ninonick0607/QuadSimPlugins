// In ROSflightControllerSource.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ROSFlightControllerSource.generated.h"

UINTERFACE(MinimalAPI)
class UROSFlightControllerSource : public UInterface
{
	GENERATED_BODY()
};

class UNREALROSFLIGHT_API IROSFlightControllerSource
{
	GENERATED_BODY()

public:
	// Use the simplest UFUNCTION() specifier for a C++ only interface.
	UFUNCTION()
	virtual float IsUsingROSflight() const = 0;

	UFUNCTION()
	virtual float GetDesiredRoll() const = 0;

	UFUNCTION()
	virtual float GetDesiredPitch() const = 0;

	UFUNCTION()
	virtual float GetDesiredYawRate() const = 0;

	UFUNCTION()
	virtual float GetDesiredThrustNormalized() const = 0;
};