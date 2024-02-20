// Fill out your copyright notice in the Description page of Project Settings.


#include "PongGameStateBase.h"
#include "Ball.h"
//#include "AIPaddle.h"
#include "Paddle.h"
#include "PongGameModeBase.h"
#include "PongHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/AudioComponent.h"
#include <GameFramework/PlayerStart.h>
#include "EndMenuWidget.h"
#include "EnergyManager.h"

APongGameStateBase::APongGameStateBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// Initializate the ball and its start location.
	TheBall = nullptr;
	BallStartLocation_Left = FVector(-1200, 0.0f, 0.0f);
	BallStartLocation_Right = FVector(1200, 0.0f, 0.0f);

	// Initialize Paddle's initial location.
	PlayerLocation_Left = FVector(-1620, 0, 0);
	PlayerLocation_Right = FVector(1620, 0, 0);
	//AILocation = FVector(0.0f, 500.0f, 170.0f);

	// Initialize Paddle's initial location.
	/*UWorld* const World = GetWorld();
	if (World)
	{
		// Get Player Start references.
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APaddle::StaticClass(), FoundActors);

		for (AActor* TActor : FoundActors)
		{
			APlayerStart* OneStartLocation = Cast<APlayerStart>(TActor);

			if (OneStartLocation->GetActorLocation().X < 0)
			{
				PlayerLocation_Left = OneStartLocation->GetActorLocation();
			}
			else if (OneStartLocation->GetActorLocation().X > 0)
			{
				PlayerLocation_Right= OneStartLocation->GetActorLocation();
			}
		}
	}*/

	// This is the first start.
	IsFirstStart = true;

	// Initial state: waiting to start.
	CurrentState = EPongStates::EWaitingToStart;

	// Initial scores.
	PlayerScore_Left  = 0;
	PlayerScore_Right = 0;

	// Maximum score.
	MaxScore = 3; 

	// How has scored last.
	LastScore = ELastScored::EPlayer_Left;

	// Get End menu class to instantiate it later and show it on the view port.
	ConstructorHelpers::FClassFinder<UUserWidget> WBPEndMenuFinder(TEXT("/Game/Blueprint/Widgets/UI/UMG_EndMenu"));
	if (WBPEndMenuFinder.Succeeded())
	{
		EndMenuClass = WBPEndMenuFinder.Class;
	}
}

void APongGameStateBase::StartGame()
{
	StopMusic();


	if (IsFirstStart)
	{
		// Get paddles references and spawn the ball.
		InitLevelActors();
		IsFirstStart = false;
	}
	// Here, we start the game after a Game Over.
	else
	{
		// Reset the scores and update the HUD.
		PlayerScore_Left = 0;
		PlayerScore_Right = 0;

		UpdateHud();

		// Change game state to Waiting to start.
		CurrentState = EPongStates::EWaitingToStart;
		
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("PlayerLocation_Left: %d %d %d"), PlayerLocation_Left.X, PlayerLocation_Left.Y, PlayerLocation_Left.Z));

		// Set paddles to their initial localtion.
		if (PlayerPaddle_Left)
			PlayerPaddle_Left->SetActorLocation(PlayerLocation_Left);
		if (PlayerPaddle_Right)
			PlayerPaddle_Right->SetActorLocation(PlayerLocation_Right);

		TheBall->ResetMaxSpeed();

		// Enable the keyboard to let the player play.
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(), 0));

		// Disable mouse cursor.
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = false;

		// Unset the game to pause.
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetPause(false);
	}

	// Launch the ball
	StartPlaying();
}

void APongGameStateBase::AddPlayerPoint(bool WasLeftPoint)
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("AddPlayerPoint.")));

	if (WasLeftPoint)
	{
		// Increase Player Score.
		PlayerScore_Left ++;

		// Update the HUD with the new score.
		UpdateHud();

		// Update who has made the last score.
		LastScore = ELastScored::EPlayer_Left;
	}
	else
	{
		// Increase Player Score.
		PlayerScore_Right ++;

		// Update the HUD with the new score.
		UpdateHud();

		// Update who has made the last score.
		LastScore = ELastScored::EPlayer_Right;
	}

	// Stop the ball and move it to its start location.
	ResetTheBall();

	// Check if we have reached the max score to end the game.
	if (IsGameEnded())
		GameEnded();

	// Automatically Launch the ball
	StartPlaying();
}

inline bool APongGameStateBase::IsGameEnded()
{
	// If someone has reached the maximum score.
	if ((PlayerScore_Left == MaxScore) || (PlayerScore_Right == MaxScore))
		return true;
	else
		return false;
}

void APongGameStateBase::GameEnded()
{
	if (EndMenuClass)
	{
		PlayMusic();

		// Get and show the end menu on view port.
		UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), EndMenuClass);
		Widget->AddToViewport();

		// Set input mode to UI to avoid continue moving the Player's paddle.
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(UGameplayStatics::GetPlayerController(GetWorld(), 0));

		// Cast the End Menu Widget to its C++ parent class to let us interact with the Widget.
		UEndMenuWidget* MenuWidget = Cast<UEndMenuWidget>(Widget);

		// Show custom end game's message.
		if (PlayerScore_Left == MaxScore)
			MenuWidget->SetEndMessage(FText::FromString("Left Player is the Kwisatz Haderach!"));
		else
			MenuWidget->SetEndMessage(FText::FromString("Right Player is the Ruler of Arrakis!"));

		// Show cursor.
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

		// Set game to paused.
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetPause(true);

		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("EnergyManager"), FoundActors);
		if (FoundActors.Num() > 0)
		{
			AEnergyManager* FoundEnergyManager = Cast<AEnergyManager>(FoundActors[0]);
			if (FoundEnergyManager)
				FoundEnergyManager->DestroySpawnedEffects();
		}
	}
}

void APongGameStateBase::PlayMusic()
{
	// Play the music sound
	if (EndMusic)
	{
		AudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), EndMusic);
		AudioComponent->SetVolumeMultiplier(0.3f); // Adjust the value as needed
	}
}

void APongGameStateBase::StopMusic()
{
	if (AudioComponent)
	{
		// Stop the music sound
		AudioComponent->Stop();
	}
}

void APongGameStateBase::UpdateHud()
{
	// Get HUD reference and update it.
	APongHUD* Hud = Cast<APongHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());

	Hud->SetPlayerLeftScored(PlayerScore_Left);
	Hud->SetPlayerRightScored(PlayerScore_Right);
}

void APongGameStateBase::StartPlaying()
{
	CurrentState = EPongStates::EPlaying;

	UWorld* const world = GetWorld();
	if (world)
	{
		// Launch the ball depending on who has made the last score.
		if (LastScore == ELastScored::EPlayer_Right)
			LaunchTheBallToPlayerLeft();
		else if (LastScore == ELastScored::EPlayer_Left)
			LaunchTheBallToPlayerRight();
		else
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("LastScore undefined")));
	}

}

void APongGameStateBase::InitLevelActors()
{
	UWorld* const World = GetWorld();
	if (World)
	{
		if (Ball_BP != NULL)
		{
			// Spawn the ball into the world.
			AActor* SpawnedActor = World->SpawnActor<AActor>(Ball_BP, BallStartLocation_Left, FRotator::ZeroRotator);
			TheBall = Cast<ABall>(SpawnedActor);
		}
		
		if (TheBall == NULL)
		{
			//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("InitLevelActors: Missing Ball BP.")));
			return;
		}

		// Get Paddle references.
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APaddle::StaticClass(), FoundActors);

		for (AActor* TActor : FoundActors)
		{
			APaddle* Paddle = Cast<APaddle>(TActor);

			if (Paddle != nullptr)
			{
				if (Paddle->GetActorLocation().X > 0)
				{
					//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("InitLevelActors: PlayerPaddle_Right.")));
					PlayerPaddle_Right = Paddle;
				}
				else if (Paddle->GetActorLocation().X < 0)
				{
					//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("InitLevelActors: PlayerPaddle_Left.")));
					PlayerPaddle_Left = Paddle;
				}
			}
		}
	}
}

void APongGameStateBase::LaunchTheBallToPlayerLeft()
{
	LaunchTheBall(-1.0f);
}

void APongGameStateBase::LaunchTheBallToPlayerRight()
{
	LaunchTheBall(1.0f);
}

void APongGameStateBase::LaunchTheBall(float XDirection)
{
	if (TheBall != nullptr)
	{
		FVector LaunchDirection(XDirection, 0.0f, 0.0f);
		TheBall->FireInDirection(LaunchDirection);
	}
	else
	{
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("LaunchBall: Ball is null.")));
	}
}

void APongGameStateBase::ResetTheBall()
{
	TheBall->StopMovement();

	// Launch the ball depending on who has made the last score.
	if (LastScore == ELastScored::EPlayer_Right)
		TheBall->SetActorLocation(BallStartLocation_Right);
	else if (LastScore == ELastScored::EPlayer_Left)
		TheBall->SetActorLocation(BallStartLocation_Left);
	else
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("LastScore undefined")));

	
}

