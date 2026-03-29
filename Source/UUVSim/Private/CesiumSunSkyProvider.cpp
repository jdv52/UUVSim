// Fill out your copyright notice in the Description page of Project Settings.


#include "CesiumSunSkyProvider.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceNull.h"
// #include "AirBlueprintLib.h"

void UCesiumSunSkyProvider::initialize()
{
    TArray<AActor*> cesium_sun_skys;
    UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), ACesiumSunSky::StaticClass(), cesium_sun_skys);

    if (cesium_sun_skys.Num() > 1)
    {
        UE_LOG(LogTemp, Warning, TEXT("More than ACesiumSunSky were found. TimeOfDay settings would be applied to first one."));
    }

    if (cesium_sun_skys.Num() > 0) {
		sun_sky_ = Cast<ACesiumSunSky>(cesium_sun_skys[0]);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("ACesiumSunSky was not found. TimeOfDay settings would not be applied."));
    }
}

void UCesiumSunSkyProvider::setTimeOfDay(uint64_t tod)
{
    if (sun_sky_) {
        FDateTime date_time = FDateTime::FromUnixTimestamp(tod);

        double solar_time = date_time.GetHour() + (date_time.GetMinute() / 60.0f) + (date_time.GetSecond() / 3600.0f);

        sun_sky_->SolarTime = solar_time;
        sun_sky_->Day = date_time.GetDay();
        sun_sky_->Month = date_time.GetMonth();
        sun_sky_->Year = date_time.GetYear();
        sun_sky_->UpdateSun();
    }
}