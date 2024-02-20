// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "EnergyObject.h" 
#include "EnergyManager.generated.h"


UCLASS()
class PONG_API AEnergyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnergyManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Timer handle for spawning EnergyObjects
	FTimerHandle SpawnTimerHandle;

	// Function to spawn an EnergyObject
	void SpawnEnergyObject();



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "EnergyManager")
	AActor* BGMusicActor;

	UPROPERTY(EditAnywhere, Category="EnergyManager")
    TSubclassOf<AActor> EnergyObjectClass;
	
	UPROPERTY(EditAnywhere, Category = "EnergyManager")
	FVector SpawnLocation = FVector(0, -100, 0);

	// To be called on Reset
	void DestroySpawnedEffects();
};
