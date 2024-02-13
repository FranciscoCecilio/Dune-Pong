// Fill out your copyright notice in the Description page of Project Settings.


#include "Frontier.h"
#include "Ball.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFrontier::AFrontier()
{
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollComp"));
	CollisionComponent->SetBoxExtent(FVector(30.0f, 30.0f, 30.0f));

	// Enable Query Only collisions in the Collision component.
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// Set collision object type to Limit.
	CollisionComponent->SetCollisionObjectType(ECC_GameTraceChannel2); // ECC_GameTraceChannel2 is Frontier Collision Object Type.
	// Reset all response channels to ignore.
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	// Set block response to Pawn's object channel.
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block); // To block Paddle objects.
	// Set block response to Projectile's object channel.
	CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block); // ECC_GameTraceChannel1 is Projectile Collision Object Type.
	// Set the method to execute when a hit event it's triggered.
	CollisionComponent->OnComponentHit.AddDynamic(this, &AFrontier::OnHit);

	RootComponent = CollisionComponent;

	// Create the sound.
	static ConstructorHelpers::FObjectFinder<USoundWave> HitSoundAsset(TEXT("/Game/Effects/pong-wall.pong-wall"));
	if (HitSoundAsset.Succeeded())
		HitSound = HitSoundAsset.Object;

}

void AFrontier::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Bound has been hit by: %s"), *OtherActor->GetName()));

		if (ABall* Ball = Cast<ABall>(OtherActor))
		{
			if (HitSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
			}
		}
	}
}