// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerKeyboard.generated.h"

/**
 * 
 */
UCLASS()
class PONG_API APlayerControllerKeyboard : public APlayerController
{
	GENERATED_BODY()
	
	virtual void SetupInputComponent() override;
};
