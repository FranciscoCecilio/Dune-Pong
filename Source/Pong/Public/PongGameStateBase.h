// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PongGameStateBase.generated.h"

class UUserWidget;
class ABall;
//class AAIPaddle;
class APaddle;

// Game states.
enum class EPongStates
{
	EWaitingToStart,
	EPlaying,
	EEnded
};

// Who has made the last score.
enum class ELastScored
{
	EPlayer_Left,
	EPlayer_Right,
	/*EArtificialIntelligence,*/
	ENone
};

/**
 *
 */
UCLASS()
class PONG_API APongGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

	APongGameStateBase();

	// Start the game.
	UFUNCTION(BlueprintCallable) // Declare the function as blueprint callable
	void StartGame();

	// Add one point to the AI because it has made a score.
	//void AddAIPoint();

	// Add one point to one of the Players because they made a score.
	void AddPlayerPoint(bool wasLeftPoint);

	// Launch the ball to the Player's paddle.
	void LaunchTheBallToPlayerLeft();

	// Launch the ball to the AI's paddle.
	void LaunchTheBallToPlayerRight();

	UPROPERTY(EditAnywhere, Category = "ActorBlueprints")
	TSubclassOf<AActor> Ball_BP;

	UPROPERTY(EditAnywhere, Category = "ActorBlueprints")
	USoundBase* EndMusic;

private:
	// The ball.
	ABall* TheBall;

	// Ball's start location in level.
	FVector BallStartLocation_Left;
	FVector BallStartLocation_Right;

	// Left Player's paddle start location.
	FVector PlayerLocation_Left;

	// Right Player's paddle start location.
	FVector PlayerLocation_Right;
	//FVector AILocation;

	// Left Player's paddle reference.
	APaddle* PlayerPaddle_Left;

	// Right Player's paddle reference.
	APaddle* PlayerPaddle_Right;
	//AAIPaddle* AIPaddle;

	// Class for end game menu.
	TSubclassOf<UUserWidget> EndMenuClass;

	// Current game state.
	EPongStates CurrentState;

	// To know if this is the first time we start the game.
	bool IsFirstStart;

	// Current Player score.
	int PlayerScore_Left;
	int PlayerScore_Right;
	//int AIScore;

	// Who has made the last score.
	ELastScored LastScore;

	// Maximum score to win the game.
	int MaxScore;

	// Get level actors references (both paddle and the ball) and set the ball to its initial location.
	void InitLevelActors();

	// Check if someone has reached the maximum score.
	bool IsGameEnded();

	// Do all the stuff related to when the game has ended: show end menu, pause the game, ...
	void GameEnded();

	UAudioComponent* AudioComponent = NULL;
	
	void PlayMusic();
	void StopMusic();

	// Update the HUD to show a new score.
	void UpdateHud();

	// Launch the ball to the player or AI depending on which one has made the last score.
	void StartPlaying();

	// Launch the ball to XDirection (to the Left or Right Player paddle).
	void LaunchTheBall(float XDirection);

	// Stop the ball and moves it to its initial location.
	void ResetTheBall();
};
