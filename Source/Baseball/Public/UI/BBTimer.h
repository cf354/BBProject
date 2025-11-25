// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BBTimer.generated.h"

class UTextBlock;

UCLASS()
class BASEBALL_API UBBTimer : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RemainingTimeText;

	void UpdateRemainTime(int32 Time);

};
