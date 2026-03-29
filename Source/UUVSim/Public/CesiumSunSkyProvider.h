// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SunSkyProvider.h"
#include "CesiumSunSky.h"

#include "CesiumSunSkyProvider.generated.h"

/**
 * 
 */
UCLASS()
class UUVSIM_API UCesiumSunSkyProvider : public UObject, public ISunSkyProvider
{
	GENERATED_BODY()

	ACesiumSunSky* sun_sky_;

public:
	void initialize();

	virtual void setTimeOfDay(uint64_t tod) override;
};
