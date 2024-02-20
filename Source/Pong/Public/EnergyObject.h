// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnergyObject.generated.h"

class UBoxComponent;

// Game states.
enum class EEnergyEffect
{
	Orange = 1,
	Yellow = 2,
	Black = 3
};


UCLASS()
class PONG_API AEnergyObject : public AActor
{
	GENERATED_BODY()
	


public:	
	// Sets default values for this actor's properties
	AEnergyObject();

private:
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* VisualComponent;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UMaterialInterface* Material1;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UMaterialInterface* Material2;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UMaterialInterface* Material3;

	 UPROPERTY(EditAnywhere)
    float VerticalVelocity = 400.0f; // Adjust the velocity as needed

	 // Sound played when the ball hits this Paddle.
	 USoundBase* HitSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetupEnergyObject();

	// This Object Effect
	EEnergyEffect CurrentEffect;
	UMaterialInterface* CurrentMaterial;

	UFUNCTION()
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
