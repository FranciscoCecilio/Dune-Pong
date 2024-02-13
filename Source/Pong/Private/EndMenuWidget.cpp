// Fill out your copyright notice in the Description page of Project Settings.


#include "EndMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "PongGameStateBase.h"
#include "Kismet/GameplayStatics.h"

void UEndMenuWidget::NativeConstruct()
{
	// On New Game button clicked, run NewGameClicked method.
	Button_NewGame->OnClicked.AddUniqueDynamic(this, &UEndMenuWidget::NewGameClicked);
}

void UEndMenuWidget::SetEndMessage(FText Message)
{
	Text_EndMessage->SetText(Message);
}

void UEndMenuWidget::NewGameClicked()
{
	// Restart the current level
	//UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()), false);

	UWorld* const world = GetWorld();
	if (world)
	{
		APongGameStateBase* GameState = Cast<APongGameStateBase>(world->GetGameState());

		RemoveFromViewport();

		GameState->StartGame();
	}
}
