// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BBPlayerController.h"

#include "UI/BBChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/BBGameModeBase.h"
#include "Player/BBPlayerState.h"
#include "UI/BBChatLog.h"
#include "Net/UnrealNetwork.h"
#include "UI/BBTimer.h"

ABBPlayerController::ABBPlayerController()
{
	bReplicates = true;
}

void ABBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	bShowMouseCursor = true;

	if (IsValid(ChatInputWidgetClass))
	{
		ChatInputWidgetInstance = CreateWidget<UBBChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance))
		{
			ChatInputWidgetInstance->AddToViewport(1);
		}
	}

	if (IsValid(NotificationTextWidgetClass))
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance))
		{
			NotificationTextWidgetInstance->AddToViewport(0);
		}
	}

	if (ChatLogWidgetClass)
	{
		ChatLogWidgetInstance = CreateWidget<UBBChatLog>(this, ChatLogWidgetClass);
		if (ChatLogWidgetInstance)
		{
			ChatLogWidgetInstance->AddToViewport(0);
		}
	}

	if (TimerWidgetClass)
	{
		TimerWidgetInstance = CreateWidget<UBBTimer>(this, TimerWidgetClass);
		if (TimerWidgetInstance)
		{
			TimerWidgetInstance->AddToViewport(0);
		}
	}
}

void ABBPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalController() == true)
	{
		ServerRPCPrintChatMessageString(InChatMessageString);
	}
}

void ABBPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	if (ChatLogWidgetInstance)
	{
		ChatLogWidgetInstance->AddChatLog(InChatMessageString);
	}
}

void ABBPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ABBPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ABBGameModeBase* BBGM= Cast<ABBGameModeBase>(GM);
		if (IsValid(BBGM) == true)
		{
			BBGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}

void ABBPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void ABBPlayerController::UpdateUITimer(int32 NewTime)
{
	if (TimerWidgetInstance)
	{
		TimerWidgetInstance->UpdateRemainTime(NewTime);
	}
}
