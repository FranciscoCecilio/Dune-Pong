// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnergyObject.h"
#include "Paddle.generated.h"

// Avoid add unnecesary imports.
class UBoxComponent;
class UPaddlePawnMovementComponent;
class USoundBase;

UCLASS()
class PONG_API APaddle : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APaddle();

protected:
	// Used to detect collisions.
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* CollisionComponent;

	// Used to show a static mesh and make it visible.
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* VisualComponent;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 100;

	// Keep current Z velocity.
	float YVelocity;

	// Use my own Pawn Movement Component implementation.
	UPaddlePawnMovementComponent* OurMovementComponent;

	// Get Pawn Movement Component.
	virtual UPawnMovementComponent* GetMovementComponent() const override;

	// Event triggered when a property has been changed in the Unreal editor.
	//virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Reference the Player2 only in Player1
	APaddle* Player2 = NULL;


public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Player's input method.
	void Move_YAxis(float AxisValue);
	void CallMoveOnPlayerTwo(float AxisValue);

	void SwapEffectUp();
	void SwapEffectDown();
	void CallSwapUpOnPlayerTwo();
	void CallSwapDownOnPlayerTwo();

	// Called for Player 1 in Level Blueprint
	UFUNCTION(BlueprintCallable)
	void AssignPlayerTwoReference(APaddle* Player2);

	/**
	* Velocity is a vector quantity that refers to "the rate at which an object changes its position."
	* From: https://www.physicsclassroom.com/class/1DKin/Lesson-1/Speed-and-Velocity
	*/
	float GetYVelocity() const;


	// This Object Effect
	EEnergyEffect CurrentEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
		UMaterialInterface* Material1;

	UPROPERTY(EditAnywhere, Category = "Effects")
		UMaterialInterface* Material2;

	UPROPERTY(EditAnywhere, Category = "Effects")
		UMaterialInterface* Material3;


private:
	// Sound played when the ball hits this paddle.
	USoundBase* HitSound;

	EEnergyEffect GetNextEnumValue();
	EEnergyEffect GetPrevEnumValue();

};
