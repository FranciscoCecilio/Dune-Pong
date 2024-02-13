// Fill out your copyright notice in the Description page of Project Settings.


#include "PongGameModeBase.h"
#include "Paddle.h"
#include "Ball.h"
#include "PongGameStateBase.h"
//#include "PongHUD.h"
#include "Kismet/GameplayStatics.h"


APongGameModeBase::APongGameModeBase()
{
	// Default Pawn for this game.
	//DefaultPawnClass = APaddle::StaticClass();

	// Game State class.
	//GameStateClass = APongGameStateBase::StaticClass();

	// HUD class this game uses.
	//HUDClass = APongHUD::StaticClass();
	
	// Get a reference to the pawn blueprint class by its path
	//UClass* PawnBlueprintClass = LoadObject<UClass>(nullptr, TEXT("/Game/Blueprint/Paddle_BP.uasset"));

	//static ConstructorHelpers::FObjectFinder<APaddle> DefaultPawnClass(TEXT("/Game/Blueprint/Paddle_BP.uasset"));
}


void APongGameModeBase::StartPlay()
{
	Super::StartPlay();
	
    // NOT HERE
	/*UWorld* const World = GetWorld();
	if (World)
	{
		APongGameStateBase* PongGameState = World->GetGameState<APongGameStateBase>();

		// Init the game on Game State.
		PongGameState->StartGame();
	}*/


}
