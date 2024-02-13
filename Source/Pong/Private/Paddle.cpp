// Fill out your copyright notice in the Description page of Project Settings.


#include "Paddle.h"
#include "Ball.h"
#include "GameFramework/HUD.h" // Include the header file for AHUD class
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PaddlePawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APaddle::APaddle()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("PaddleCollisionComponent"));
	VisualComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PaddleVisualComponent"));

	RootComponent = CollisionComponent;

	// Set up CollisionComponent as parent of VisualComponent;
	VisualComponent->SetupAttachment(CollisionComponent);

	// Disable collisions on Visual Component using "NoCollision" profile.
	VisualComponent->BodyInstance.SetCollisionProfileName("NoCollision");
	// Enable collisions on Collision Component using "Pawn" profile.
	CollisionComponent->BodyInstance.SetCollisionProfileName("Pawn");
	// Add method to manage collisions.
	CollisionComponent->OnComponentHit.AddDynamic(this, &APaddle::OnHit);

	// Initialize paddle velocity.
	YVelocity = 0.0f;

	// Create an instance of our movement component, and tell it to update our root component.
	OurMovementComponent = CreateDefaultSubobject<UPaddlePawnMovementComponent>(TEXT("PaddleCustomMovementComponent"));
	// Set the component that this movement component will move.
	OurMovementComponent->UpdatedComponent = RootComponent;

	// Create the sound to play it when the ball hit this paddle.
	static ConstructorHelpers::FObjectFinder<USoundWave> HitSoundAsset(TEXT("/Game/Effects/pong-paddle.pong-paddle"));
	if (HitSoundAsset.Succeeded())
		HitSound = HitSoundAsset.Object;
	else
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("HitSound not found: Game/Effects/pong-paddle.pong-paddle")));

	// Set initial effect
	CurrentEffect = EEnergyEffect::Black;
}

UPawnMovementComponent* APaddle::GetMovementComponent() const
{
	return OurMovementComponent;
}

/*void APaddle::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// Check if the property changed is valid.
	if (PropertyChangedEvent.Property != nullptr)
	{
		// Get the name of the changed property
		const FName PropertyName(PropertyChangedEvent.Property->GetFName());

		// If the changed property is Static Mesh then we can set
		// CollisionComponent extend to the same size of the new Static Mesh
		// set to the VisualComponent.
		if (PropertyName == GET_MEMBER_NAME_CHECKED(APaddle, VisualComponent))
		{
			// Get the new Static Mesh set to the Visual Component.
			UStaticMesh* SM = VisualComponent->GetStaticMesh();
			// If it is valid.
			if (SM != nullptr)
			{
				// Get the Static Mesh Bounds.
				FBoxSphereBounds Bounds = SM->ExtendedBounds;

				// Set this Bounds to the Collision Component to 
				CollisionComponent->SetBoxExtent(Bounds.BoxExtent);
			}
		}
	}

	// Then call the parent version of this function
	Super::PostEditChangeProperty(PropertyChangedEvent);
}*/

void APaddle::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Paddle has been hit by: %s"), *OtherActor->GetName()));

	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{

		// If the ball has hit this paddle, play a sound.
		if (ABall* Ball = Cast<ABall>(OtherActor))
		{
			if (HitSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
			}
		}
	}
}

// Called to bind functionality to input
void APaddle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	int id = -1;
	if (Controller)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		if (PlayerController)
		{
			id = PlayerController->GetLocalPlayer()->GetControllerId();
		}
	}

	if (id == 0)
	{
		// Bind Player 1 (W/S)
		InputComponent->BindAxis(TEXT("MovePaddleKeyboard1"), this, &APaddle::Move_YAxis);
		// Bind Player 1 (A/D)
		InputComponent->BindAction(TEXT("SwapUpKeyboard1"), IE_Pressed, this, &APaddle::SwapEffectUp);
		InputComponent->BindAction(TEXT("SwapDownKeyboard1"), IE_Pressed, this, &APaddle::SwapEffectDown);

		// Bind Player 2 (Up/Down)
		InputComponent->BindAxis(TEXT("MovePaddleKeyboard2"), this, &APaddle::CallMoveOnPlayerTwo);
		// Bind Player 2 (Left/Right)
		InputComponent->BindAction(TEXT("SwapUpKeyboard2"), IE_Pressed, this, &APaddle::CallSwapUpOnPlayerTwo);
		InputComponent->BindAction(TEXT("SwapDownKeyboard2"), IE_Pressed, this, &APaddle::CallSwapDownOnPlayerTwo);
		
		// Alternatively, for Player 2
		// Bind Player 2 (Dualshock PS4 - Right thumbstick)
		InputComponent->BindAxis(TEXT("MovePaddleController"), this, &APaddle::CallMoveOnPlayerTwo);
		InputComponent->BindAction(TEXT("SwapUpController"), IE_Pressed, this, &APaddle::CallSwapUpOnPlayerTwo);
		InputComponent->BindAction(TEXT("SwapDownController"), IE_Pressed, this, &APaddle::CallSwapDownOnPlayerTwo);
	}
	else if(id > 0) // Irrelevant because only Player 1 will have a "working" Player Controller
	{
		// Bind "MovePaddle" Axis Mapping with APaddle::Move_YAxis method.
		InputComponent->BindAxis(TEXT("MovePaddleKeyboard2"), this, &APaddle::Move_YAxis);
		InputComponent->BindAxis(TEXT("MovePaddleController"), this, &APaddle::Move_YAxis);
	}
	else
	{
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("something went wrong with player controller IDs")));
	}
}


void APaddle::AssignPlayerTwoReference(APaddle* SecondPlayer)
{
	Player2 = SecondPlayer;
}


void APaddle::Move_YAxis(float AxisValue)
{
	// Create a controller deadzone to prevent moving without input
	if (FMath::Abs(AxisValue) > 0.1f)
	{
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, .1f, FColor::Green, FString::Printf(TEXT("AxisValue: %f"), AxisValue));

		// Keep the lastest Y velocity to check it when the ball hit this paddle.
		YVelocity = AxisValue;

		FVector DirectionVector = FVector(0.0f,  AxisValue, 0.0f);

		OurMovementComponent->AddInputVector(DirectionVector * MoveSpeed);
	}
}

void APaddle::CallMoveOnPlayerTwo(float AxisValue)
{
	if (Player2)
	{
		Player2->Move_YAxis(AxisValue);
	}
}

void APaddle::SwapEffectUp()
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, .1f, FColor::Green, FString::Printf(TEXT("SwapEffect Up")));
	CurrentEffect = GetNextEnumValue();

	if (static_cast<int>(CurrentEffect) == 1)
	{
		VisualComponent->SetMaterial(0, Material1);
	}
	else if (static_cast<int>(CurrentEffect) == 2)
	{
		VisualComponent->SetMaterial(0, Material2);
	}
	else if (static_cast<int>(CurrentEffect) == 3)
	{
		VisualComponent->SetMaterial(0, Material3);
	}
}

void APaddle::SwapEffectDown()
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, .1f, FColor::Green, FString::Printf(TEXT("SwapEffect Down")));
	CurrentEffect = GetPrevEnumValue();

	if (static_cast<int>(CurrentEffect) == 1)
	{
		VisualComponent->SetMaterial(0, Material1);
	}
	else if (static_cast<int>(CurrentEffect) == 2)
	{
		VisualComponent->SetMaterial(0, Material2);
	}
	else if (static_cast<int>(CurrentEffect) == 3)
	{
		VisualComponent->SetMaterial(0, Material3);
	}
}


void APaddle::CallSwapUpOnPlayerTwo()
{
	if (Player2)
	{
		Player2->SwapEffectUp();
	}
}

void APaddle::CallSwapDownOnPlayerTwo()
{
	if (Player2)
	{
		Player2->SwapEffectDown();
	}
}

// Function to get the next enum value
EEnergyEffect APaddle::GetNextEnumValue()
{
	// Increment the enum value
	int nextValue = static_cast<int>(CurrentEffect) + 1;

	// Handle boundary condition
	if (nextValue > 3)
	{
		// Wrap around to the first enum value
		nextValue = 1;
	}

	// Return the next enum value
	return static_cast<EEnergyEffect>(nextValue);
}


// Function to get the next enum value
EEnergyEffect APaddle::GetPrevEnumValue()
{
	// Increment the enum value
	int nextValue = static_cast<int>(CurrentEffect) - 1;

	// Handle boundary condition
	if (nextValue < 1)
	{
		// Wrap around to the first enum value
		nextValue = 3;
	}

	// Return the next enum value
	return static_cast<EEnergyEffect>(nextValue);
}



float APaddle::GetYVelocity() const
{
	return YVelocity;
}
