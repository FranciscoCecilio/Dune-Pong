// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndMenuWidget.generated.h"

class UTextBlock;
class UButton;

/**
 *
 */
UCLASS()
class PONG_API UEndMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void NativeConstruct() override;

	// Set the end message: if the player has won or if she/he has lost.
	void SetEndMessage(FText Message);

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Text_EndMessage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_NewGame;

	UFUNCTION()
	void NewGameClicked();

};
