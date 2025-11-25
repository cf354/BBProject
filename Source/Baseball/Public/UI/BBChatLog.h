// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BBChatLog.generated.h"

class UTextBox;
class UScrollBox;

UCLASS()
class BASEBALL_API UBBChatLog : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void AddChatLog(const FString& ChatMessage);

protected:
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ScrollBox_ChatLog;
};
