// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"
#include "Components/TextBlock.h"

void UPlayerHUDWidget::SetPlayerLeftScore(int NewScore)
{
	Left_Score->SetText(FText::AsNumber(NewScore));
}

void UPlayerHUDWidget::SetPlayerRightScore(int NewScore)
{
	Right_Score->SetText(FText::AsNumber(NewScore));
}

void UPlayerHUDWidget::ClearScore()
{
	Left_Score->SetText(FText::FromString(TEXT("")));
	Right_Score->SetText(FText::FromString(TEXT("")));
}
