// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyObject.h"
#include "Ball.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnergyObject::AEnergyObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	VisualComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualComp"));
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollComp"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> VisualAsset(TEXT("/Game/Geometry/Meshes/Ball_mesh.Ball_mesh"));

	if (VisualAsset.Succeeded())
	{
		VisualComponent->SetStaticMesh(VisualAsset.Object);
		VisualComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		VisualComponent->SetWorldScale3D(FVector(5.0f));

		FBoxSphereBounds Bounds = VisualAsset.Object->ExtendedBounds;

		CollisionComponent->SetRelativeLocation(Bounds.Origin);
		CollisionComponent->SetBoxExtent(Bounds.BoxExtent);
	}

	// Set Collision component as Root Component.
	RootComponent = CollisionComponent;
	// Attach Visual Component to Collision Component.
	VisualComponent->SetupAttachment(CollisionComponent);

	// Enable only Query Collisions in the Collision component.
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// Set collision object type to Objective
	CollisionComponent->SetCollisionObjectType(ECC_GameTraceChannel4); // ECC_GameTraceChannel4 is Effect Collision Object Type.
	// Set to generate overlap events.
	CollisionComponent->SetGenerateOverlapEvents(true);

	// Create the sound.
	static ConstructorHelpers::FObjectFinder<USoundWave> HitSoundAsset(TEXT("/Game/Effects/pong-effect.pong-effect"));
	if (HitSoundAsset.Succeeded())
		HitSound = HitSoundAsset.Object;


	// Set default materials
	Material1 = nullptr; 
	Material2 = nullptr;
	Material3 = nullptr;
}

// Called when the game starts or when spawned
void AEnergyObject::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void AEnergyObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Move the actor upwards with constant velocity
	FVector NewLocation = GetActorLocation();
	NewLocation.Y += VerticalVelocity * DeltaTime;
	SetActorLocation(NewLocation);
}

void AEnergyObject::SetupEnergyObject()
{
	float random = FMath::RandRange(1, 3);

	if (random == 1)
	{
		CurrentEffect = EEnergyEffect::Orange;
		// Set mesh material
		if (Material1)
		{
			VisualComponent->SetMaterial(0, Material1); // You may need to adjust the material index
			CurrentMaterial = Material1;
		}

	}
	else if (random == 2)
	{
		CurrentEffect = EEnergyEffect::Yellow;
		// Set mesh material
		if (Material2)
		{
			VisualComponent->SetMaterial(0, Material2); // You may need to adjust the material index
			CurrentMaterial = Material2;
		}
	}
	else if (random == 3)
	{
		CurrentEffect = EEnergyEffect::Black;
		// Set mesh material
		if (Material3)
		{
			VisualComponent->SetMaterial(0, Material3); // You may need to adjust the material index
			CurrentMaterial = Material3;
		}
	}
}

void AEnergyObject::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("YEYYYYY")));

	ABall* Ball = Cast<ABall>(OtherActor);
	if (Ball)
	{
		if (HitSound!= nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		}

		Ball->SetNewEnergyEffect(CurrentEffect, CurrentMaterial);
		
		// Selfdestruct 
		Destroy();
	}


}

