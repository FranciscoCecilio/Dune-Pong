// Fill out your copyright notice in the Description page of Project Settings.


#include "PongHUD.h"
#include "PongGameStateBase.h"
#include "PlayerHUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Canvas.h"
#include "Blueprint/UserWidget.h"
#include "UObject/Class.h"

const float MidScreenOffset = 80.0f;


APongHUD::APongHUD()
{
	// Get the score font.
	ConstructorHelpers::FObjectFinder<UFont> ScoreFontObject(TEXT("Font'/Game/Fonts/ScoreFont.ScoreFont'"));
	if (ScoreFontObject.Object)
		ScoreFont = ScoreFontObject.Object;
	else
		ScoreFont = nullptr;

	// Get the text font.
	ConstructorHelpers::FObjectFinder<UFont> TextFontObject(TEXT("Font'/Game/Fonts/TextFont.TextFont'"));
	if (TextFontObject.Object)
		TextFont = TextFontObject.Object;
	else
		TextFont = nullptr;

	// Load the level HUD, where we display the score, to get its class.
	ConstructorHelpers::FClassFinder<UUserWidget> WBPPlayerHudFinder(TEXT("/Game/Blueprint/Widgets/UI/UMG_PlayerHUD"));
	if (WBPPlayerHudFinder.Succeeded())
		HudClass = WBPPlayerHudFinder.Class;
	else
		HudClass = nullptr;
}

// Called when the game starts or when spawned
void APongHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HudClass)
	{
		Widget = CreateWidget<UUserWidget>(GetWorld(), HudClass);

		Widget->AddToViewport();

		PlayerHudWidget = Cast<UPlayerHUDWidget>(Widget);
	}
}

void APongHUD::SetPlayerRightScored(int value)
{
	if (PlayerHudWidget != nullptr && IsVisible)
	{
		PlayerHudWidget->SetPlayerRightScore(value);
	}
}

void APongHUD::SetPlayerLeftScored(int value)
{
	if (PlayerHudWidget != nullptr && IsVisible)
	{
		PlayerHudWidget->SetPlayerLeftScore(value);
	}
}


void APongHUD::HideThisHUD()
{
	IsVisible = false;

	if (PlayerHudWidget != nullptr)
	{
		PlayerHudWidget->ClearScore();
	}
}
