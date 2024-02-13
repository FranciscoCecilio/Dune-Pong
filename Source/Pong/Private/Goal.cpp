// Fill out your copyright notice in the Description page of Project Settings.


#include "Goal.h"
#include "Ball.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGoal::AGoal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create collision component.
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollComp"));
	// Set Collision's component size.
	CollisionComponent->SetBoxExtent(FVector(30.0f, 30.0f, 30.0f));

	// Enable only Query Collisions in the Collision component.
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// Set collision object type to Objective
	CollisionComponent->SetCollisionObjectType(ECC_GameTraceChannel3); // ECC_GameTraceChannel3 is Objective Collision Object Type.
	// Set all response channels to ignore.d
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	// Set overlap response to Objective's object channel.
	CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap); // ECC_GameTraceChannel1 is Projectile Collision Object Type.
	// Set to generate overlap events.
	CollisionComponent->SetGenerateOverlapEvents(true);

	// Set Collision component as Root Component.
	RootComponent = CollisionComponent;

	// Create the sound.
	static ConstructorHelpers::FObjectFinder<USoundWave> HitSoundAsset(TEXT("/Game/Effects/pong-point.pong-point"));
	if (HitSoundAsset.Succeeded())
		HitSound = HitSoundAsset.Object;

}

void AGoal::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// When the ball hits this, play a sound.
	if (ABall* Ball = Cast<ABall>(OtherActor))
	{
		if (HitSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		}
	}
}

bool AGoal::GetIsRight()
{
	return IsRight;
}



