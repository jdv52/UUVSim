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

    if (actor->IsA(ARovPawn::StaticClass())) {
        // do nothing

        // UE_LOG(LogTemp, Warning, TEXT("CesiumIntegrationLayer: Actor spawned: %s"), *actor->GetName());

		// auto rov_pawn = Cast<ARovPawn>(actor);

		// RovPawnSpawnHook(rov_pawn);
    }
    else if (actor->IsA(ASimModeBase::StaticClass())) {
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

void UCesiumIntegrationLayer::RovPawnSpawnHook(ARovPawn* rov_pawn, ASimModeBase* owner)
{
    TArray<UCesiumGlobeAnchorComponent*> globe_anchor_components;
    rov_pawn->GetComponents<UCesiumGlobeAnchorComponent>(globe_anchor_components);

    if (globe_anchor_components.Num() == 0) {
        UE_LOG(LogTemp, Warning, TEXT("CesiumIntegrationLayer: Adding CesiumGlobeAnchorComponent to %s"), *rov_pawn->GetName());

        auto globe_anchor_component = NewObject<UCesiumGlobeAnchorComponent>(rov_pawn);

        if (globe_anchor_component)
        {
            globe_anchor_component->RegisterComponent();
            rov_pawn->AddInstanceComponent(globe_anchor_component);

            TArray<AActor*> georeferences;
            UGameplayStatics::GetAllActorsOfClass(rov_pawn->GetWorld(), ACesiumSunSky::StaticClass(), georeferences);

            if (georeferences.Num() > 0) {
                auto georeference = Cast<ACesiumGeoreference>(georeferences[0]);
                globe_anchor_component->SetGeoreference(georeference);
            }

            globe_anchor_component->MoveToLongitudeLatitudeHeight(
                FVector(
                    owner->getSettings().origin_geopoint.home_geo_point.latitude,
                    owner->getSettings().origin_geopoint.home_geo_point.longitude,
                    owner->getSettings().origin_geopoint.home_geo_point.altitude
                )
            );
        }
    }
}

void UCesiumIntegrationLayer::SimModeBaseSpawnHook(ASimModeBase* sim_mode_base)
{
    auto sun_sky_provider = NewObject<UCesiumSunSkyProvider>(sim_mode_base);
    sun_sky_provider->initialize();

    sim_mode_base->setSunSkyProvider(sun_sky_provider);

	TArray<AActor*> vehicle_pawns;
    sim_mode_base->getExistingVehiclePawns(vehicle_pawns);

    for (auto pawn : vehicle_pawns)
    {
        if (pawn->IsA(ARovPawn::StaticClass())) {
			auto rov_pawn = Cast<ARovPawn>(pawn);
			RovPawnSpawnHook(rov_pawn, sim_mode_base);
        }
    }
}