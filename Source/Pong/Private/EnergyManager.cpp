// Fill out your copyright notice in the Description page of Project Settings.

#include "EnergyManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnergyManager::AEnergyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnergyManager::BeginPlay()
{
	Super::BeginPlay();

    // Set up the timer for spawning EnergyObjects
    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AEnergyManager::SpawnEnergyObject, 2.0f, true); // Spawn every 2 seconds, adjust as needed
	
}

// Called every frame
void AEnergyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnergyManager::SpawnEnergyObject()
{
    // Spawn the EnergyObject
    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(EnergyObjectClass, SpawnLocation, FRotator::ZeroRotator);

    AEnergyObject* EnergyObject  = Cast<AEnergyObject>(SpawnedActor);
    if (EnergyObject)
    {
        // Optionally, you can set up additional logic for the spawned EnergyObject
        EnergyObject->SetupEnergyObject();
    }
}

void AEnergyManager::DestroySpawnedEffects()
{
    // Get EnergyActor references. 
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnergyObject::StaticClass(), FoundActors);

    for (AActor* TActor : FoundActors)
    {
        AEnergyObject* EnergyActor = Cast<AEnergyObject>(TActor);

        if (EnergyActor != nullptr)
        {
            // Destroy the actor.
            EnergyActor->Destroy();
        }
    }
    
    // Also restart BG music
    // Get the audio component from the actor
    UAudioComponent* MyAudioComponent = BGMusicActor->FindComponentByClass<UAudioComponent>();
    if (MyAudioComponent)
    {
        MyAudioComponent->Stop();
        MyAudioComponent->Play();
        //if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString::Printf(TEXT("DestroySpawnedEffects. Reset")));
    }
    else
    {
        //if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("DestroySpawnedEffects. Cast falhou...")));
    }
}
