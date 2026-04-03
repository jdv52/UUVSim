// Fill out your copyright notice in the Description page of Project Settings.


#include "CesiumIntegrationLayer.h"
#include "Kismet/GameplayStatics.h"
#include "CesiumSunSkyProvider.h"
#include "CesiumGeoreference.h"
#include "CesiumGlobeAnchorComponent.h"

void UCesiumIntegrationLayer::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

	// Wait for world to be fully initialized before doing anything
    FWorldDelegates::OnPostWorldInitialization.AddUObject(
        this,
		&UCesiumIntegrationLayer::OnWorldPostInitialization
    );
}

void UCesiumIntegrationLayer::Deinitialize()
{
    Super::Deinitialize();

    UE_LOG(LogTemp, Warning, TEXT("MyWorldSubsystem deinitialized!"));
}

void UCesiumIntegrationLayer::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

    UE_LOG(LogTemp, Warning, TEXT("CesiumIntegrationLayer: World has begun play!"));
}

void UCesiumIntegrationLayer::OnActorSpawned(AActor* actor)
{
    if (!actor)
        return;

    if (actor->IsA(ASimModeBase::StaticClass())) {
        UE_LOG(LogTemp, Warning, TEXT("CesiumIntegrationLayer: Setting SimModeBase sun sky provider"));

        ASimModeBase* sim_mode = Cast<ASimModeBase>(actor);

		SimModeBaseSpawnHook(sim_mode);
    }
}

void UCesiumIntegrationLayer::OnWorldPostInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
    if (!World) {
        return;
    }

    if (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE) {
        UE_LOG(LogTemp, Warning, TEXT("CesiumIntegrationLayer: World %s (type %d) post-initialization!"), *World->GetName(), World->WorldType);

        World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UCesiumIntegrationLayer::OnActorSpawned));
    }
}

void UCesiumIntegrationLayer::SimModeBaseSpawnHook(ASimModeBase* sim_mode_base)
{
    auto sun_sky_provider = NewObject<UCesiumSunSkyProvider>(sim_mode_base);
    sun_sky_provider->initialize();

    sim_mode_base->setSunSkyProvider(sun_sky_provider);

	TArray<AActor*> vehicle_pawns;
    sim_mode_base->getExistingVehiclePawns(vehicle_pawns);

    TArray<AActor*> georeferences;
    UGameplayStatics::GetAllActorsOfClass(sim_mode_base->GetWorld(), ACesiumGeoreference::StaticClass(), georeferences);

    if (georeferences.Num() > 0) {
        auto georeference = Cast<ACesiumGeoreference>(georeferences[0]);

		auto origin_geopoint = sim_mode_base->getSettings().origin_geopoint.home_geo_point;

		FVector origin_location = FVector(
			origin_geopoint.longitude,
			origin_geopoint.latitude,
			origin_geopoint.altitude
		);


        UE_LOG(LogTemp, Warning, TEXT("CesiumIntegrationLayer: Setting georeference origin to %s"), *origin_location.ToString());

        georeference->SetOriginLongitudeLatitudeHeight(
            origin_location
        );
    }
}