// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Goal.generated.h"

// Avoid add unnecesary imports.
class UBoxComponent;
class USoundBase;

UCLASS()
class PONG_API AGoal : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGoal();



protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, Category = "Goal")
	bool IsRight;



public:

	UFUNCTION()
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	bool GetIsRight();

private:
	// Sound played when the ball hits this Paddle.
	USoundBase* HitSound;

};
