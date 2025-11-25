// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/BBChatLog.h"

#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void UBBChatLog::AddChatLog(const FString& ChatMessage)
{
	if (!ScrollBox_ChatLog) return;

	UTextBlock* NewText = NewObject<UTextBlock>(ScrollBox_ChatLog);
	NewText->SetText(FText::FromString(ChatMessage));

	ScrollBox_ChatLog->AddChild(NewText);
	ScrollBox_ChatLog->ScrollToEnd();
}