// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

/**
 *
 */
UCLASS()
class PONG_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Left_Score;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* Right_Score;

	// Set 'NewScore' as the score for the Player.
	void SetPlayerLeftScore(int NewScore);

	// Set 'NewScore' as the score for the AI.
	void SetPlayerRightScore(int NewScore);

	// Clears the text scores (used to hide this HUD)
	void ClearScore();
};
