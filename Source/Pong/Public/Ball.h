// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnergyObject.h"
#include "Ball.generated.h"

// Avoid add unnecesary imports.
class UBoxComponent;
class UProjectileMovementComponent;
class USoundBase;

UCLASS()
class PONG_API ABall : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABall();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* VisualComponent;

	UPROPERTY(VisibleAnywhere, Category = "MyMovement")
	UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditAnywhere, Category = "MyMovement")
	float InitialSpeed = 0.0f;
	UPROPERTY(EditAnywhere, Category = "MyMovement")
	float MaxSpeed = 190.0f;
	UPROPERTY(EditAnywhere, Category = "MyMovement")
	float Bounciness = 0.3f;

	
	//virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:

	// Function that initializes the ball's velocity in the shoot direction.
	void FireInDirection(const FVector& ShootDirection);

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	// Returns ball Velocity vector.
	FVector GetVelocity() const;

	// Stops the ball setting its Projectile Movement Component Velocity vector to ZeroVector.
	void StopMovement();

	// This Object Effect
	EEnergyEffect CurrentEffect;

	void SetNewEnergyEffect(EEnergyEffect NewEffect, UMaterialInterface* NewMat);

	void ResetMaxSpeed();

private:
	// Returns 'value' to a value between [-1.0, 1.0].
	float Reduce(float value) const;

	// Sounds
	USoundBase* HitSoundPaddleHit;
	USoundBase* HitSoundPoint;

};