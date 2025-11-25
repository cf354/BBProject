// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BBTimer.h"

#include "Components/TextBlock.h"


void UBBTimer::UpdateRemainTime(int32 Time)
{
	if (RemainingTimeText)
	{
		RemainingTimeText->SetText(FText::AsNumber(Time));
	}
}
