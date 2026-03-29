// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SimMode/SimModeBase.h"
#include "Vehicles/Rov/RovPawn.h"

#include "CesiumIntegrationLayer.generated.h"

/**
 * 
 */
UCLASS()
class UUVSIM_API UCesiumIntegrationLayer : public UWorldSubsystem
{
	GENERATED_BODY()

	void RovPawnSpawnHook(ARovPawn* rov_pawn, ASimModeBase* owner);

	void SimModeBaseSpawnHook(ASimModeBase* sim_mode_base);
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	void OnWorldPostInitialization(UWorld* World, const UWorld::InitializationValues IVS);

	void OnActorSpawned(AActor* actor);

};
