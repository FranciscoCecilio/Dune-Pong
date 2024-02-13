// Fill out your copyright notice in the Description page of Project Settings.


#include "Ball.h"
#include "Paddle.h"
#include "Goal.h"
#include "PongGameStateBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Kismet/GameplayStatics.h>

template<typename Base, typename T>
inline bool instanceof(const T*) {
	return std::is_base_of<Base, T>::value;
}

// Sets default values
ABall::ABall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	VisualComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualComp"));
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollComp"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> VisualAsset(TEXT("/Game/Geometry/Meshes/Ball_mesh.Ball_mesh"));

	if (VisualAsset.Succeeded())
	{
		VisualComponent->SetStaticMesh(VisualAsset.Object);
		VisualComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		VisualComponent->SetWorldScale3D(FVector(1.0f));

		FBoxSphereBounds Bounds = VisualAsset.Object->ExtendedBounds;

		CollisionComponent->SetRelativeLocation(Bounds.Origin);
		CollisionComponent->SetBoxExtent(Bounds.BoxExtent);
	}

	// Set Collision component as Root Component.
	RootComponent = CollisionComponent;
	// Attach Visual Component to Collision Component.
	VisualComponent->SetupAttachment(CollisionComponent);

	// Disable collisions in Visual Component.
	VisualComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Enable only Query Collisions in the Collision component.
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// Set collision object type to Projectile.
	CollisionComponent->SetCollisionObjectType(ECC_GameTraceChannel1); // ECC_GameTraceChannel1 is Projectile Collision Object Type.
	// Set all response channels to ignore.
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	// Set block response to Pawn's object channel.
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	// Set block response to Frontier's object channel.
	CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block); // ECC_GameTraceChannel2 is Frontier Collision Object Type.
	// Set overlap response to Objective's object channel.
	CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap); // ECC_GameTraceChannel3 is Objective Collision Object Type.
	CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Overlap); // ECC_GameTraceChannel4 is Effect Collision Object Type.
	// Set to generate overlap events.
	CollisionComponent->SetGenerateOverlapEvents(true);
	// Set the method to execute when a hit event it's triggered.
	CollisionComponent->OnComponentHit.AddDynamic(this, &ABall::OnHit);
	
	// Use this component to drive this ball's movement.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	// Set the component that this movement component will move.
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = MaxSpeed;
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = Bounciness;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	// Set initial effect
	CurrentEffect = EEnergyEffect::Black;

	// Create the sound.
	static ConstructorHelpers::FObjectFinder<USoundWave> HitSoundAsset(TEXT("/Game/Effects/pong-point.pong-point"));
	if (HitSoundAsset.Succeeded())
		HitSoundPoint = HitSoundAsset.Object;

	// Create the sound.
	static ConstructorHelpers::FObjectFinder<USoundWave> HitSoundAsset2(TEXT("/Game/Effects/pong-paddle.pong-paddle"));
	if (HitSoundAsset2.Succeeded())
		HitSoundPaddleHit = HitSoundAsset2.Object;
}

/*void ABall::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// Check if property is valid
	if (PropertyChangedEvent.Property != nullptr)
	{
		// Get the name of the changed property
		const FName PropertyName(PropertyChangedEvent.Property->GetFName());
		// If the changed property is ShowStaticMesh then we
		// will set the visibility of the actor
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ABall, VisualComponent))
		{
			UStaticMesh* SM = VisualComponent->GetStaticMesh();
			if (SM != nullptr)
			{
				FBoxSphereBounds Bounds = SM->ExtendedBounds;

				CollisionComponent->SetRelativeLocation(Bounds.Origin);
				CollisionComponent->SetBoxExtent(Bounds.BoxExtent);
			}
		}
	}
	// Then call the parent version of this function
	Super::PostEditChangeProperty(PropertyChangedEvent);
}*/

void ABall::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->MaxSpeed;
}

void ABall::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Ball Hits: %s"), *OtherActor->GetName()));

		// Compute Reflected Velocity depending on the actor with whom we have collided.
		FVector ReflectedVelocity;

		// Paddle Hit.
		if (APaddle* Paddle = Cast<APaddle>(OtherActor))
		{
			// If CurrentEffect is Wrong
			if (Paddle->CurrentEffect != CurrentEffect)
			{
				UWorld* const world = GetWorld();
				if (world)
				{
					APongGameStateBase* GameState = Cast<APongGameStateBase>(world->GetGameState());
					if (GetActorLocation().X < 0)
					{
						// Add one point to Left score.
						GameState->AddPlayerPoint(true);
					}
					else
					{
						// Add one point to Right score.
						GameState->AddPlayerPoint(false);
					}

					if (HitSoundPoint != nullptr)
					{
						UGameplayStatics::PlaySoundAtLocation(this, HitSoundPoint, GetActorLocation());
					}
				}
			}
			else
			{
				if (HitSoundPaddleHit != nullptr)
				{
					UGameplayStatics::PlaySoundAtLocation(this, HitSoundPaddleHit, GetActorLocation());
				}
			}
			

			float PaddleVelocity = Paddle->GetYVelocity();

			// The ball only moves on axis X, and Y. (NOT Z)
			// Here, X = -1.0 because this paddle is on the left side and the ball will bounce to the right side of the screen.
			// PaddleVelocity is Paddle's Y velocity.
			// If Paddle is moving down (PaddleVelocity < 0.0), the ball will bounces down.
			// If Paddle is moving up (PaddleVelocity > 0.0), the ball will bounces up.
			// This is why I use PaddleVelocity as the new Y velocity for the ball.

			float XDirection; // 1.0f: Left side, and -1.0f: Right side

			if (GetActorLocation().X < 0)
			{
				XDirection = 1;
			}
			else
			{
				XDirection = -1;
			}

			FVector Direction = FVector(XDirection, /*ProjectileMovementComponent->Velocity.Y*/ PaddleVelocity, 0.0f /*1.0f*/);

			ReflectedVelocity = Direction * ProjectileMovementComponent->MaxSpeed;

			// BONUS: Increase Ball speed!
			ProjectileMovementComponent->MaxSpeed += 15;
		}
		// Frontier Hit.
		else
		{
			FVector BallVelocity = ProjectileMovementComponent->Velocity;
			// The ball will continue moving on the same direction on Y axis.
			// But it will change its direction on Z axis because the frontiers are at the top and at the bottom of the screen.
			// This is why we change the sign of BallVelocity.Z.
			// The Z axis is the vertical axis of the screen(up and down).
			ReflectedVelocity = FVector(BallVelocity.X, -BallVelocity.Y, 0.0f);
		}


		// This happens when the ball hits a paddle that is not moving up or down.
		if (FMath::Abs(ReflectedVelocity.Y) < 1)
		{
			float random = FMath::RandRange(-1.0f, 1.0f);

			// if it is negative, it will move down.
			/*if (random < 0.0f)
				ReflectedVelocity.Y = -1.0f * ProjectileMovementComponent->MaxSpeed;
			// else, if it is positive, it will move up.
			else
				ReflectedVelocity.Y = 1.0f * ProjectileMovementComponent->MaxSpeed;
			*/
			ReflectedVelocity.Y = random * ProjectileMovementComponent->MaxSpeed;


			//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Y was zero.")));

		}
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("ReflectedVelocity X: %f  Y:%f "), ReflectedVelocity.X, ReflectedVelocity.Y));

		// Set the new velocity to the ball's movement component.
		ProjectileMovementComponent->Velocity = ReflectedVelocity;

		UE_LOG(LogTemp, Warning, TEXT("Current values are: ReflectedVelocity %s"), *ReflectedVelocity.ToString());
	}
}

void ABall::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, FString::Printf(TEXT("Name %s"), *OtherActor->GetName()));

	AGoal* GoalActor = Cast<AGoal>(OtherActor);
	if (GoalActor)
	{
		// If the ball has overlaped the Left Goal, then Right makes a point.
		if (GoalActor->GetIsRight() == false) // LEFT
		{
			UWorld* const world = GetWorld();
			if (world)
			{
				APongGameStateBase* GameState = Cast<APongGameStateBase>(world->GetGameState());

				// Add one point to Left score.
				GameState->AddPlayerPoint(false);
			}
		}
		// If the ball has overlaped the Player Goal, then Left Player makes a point.
		else if (GoalActor->GetIsRight() == true)// RIGHT
		{
			UWorld* const world = GetWorld();
			if (world)
			{
				APongGameStateBase* GameState = Cast<APongGameStateBase>(world->GetGameState());

				// Add one point to Right player score.
				GameState->AddPlayerPoint(true);
			}
		}
	}

	// Nao entra aqui.
	AEnergyObject* EnergyObject = Cast<AEnergyObject>(OtherActor);
	if (EnergyObject)
	{
		// Change this material and Effect
		CurrentEffect = EnergyObject->CurrentEffect;
		VisualComponent->SetMaterial(0, EnergyObject->CurrentMaterial);
	}
	
}

void ABall::SetNewEnergyEffect(EEnergyEffect NewEffect, UMaterialInterface* NewMat)
{
	// Change this material and Effect
	CurrentEffect = NewEffect;
	VisualComponent->SetMaterial(0, NewMat);
}

void ABall::ResetMaxSpeed()
{
	ProjectileMovementComponent->MaxSpeed = MaxSpeed;
}

FVector ABall::GetVelocity() const
{
	return ProjectileMovementComponent->Velocity;
}

void ABall::StopMovement()
{
	ProjectileMovementComponent->Velocity = FVector::ZeroVector;
}

float ABall::Reduce(float value) const
{
	if (value >= 1.0f)
		return 1.0f;
	else if (value <= -1.0f)
		return -1.0f;
	else
		return 0.0f;
}