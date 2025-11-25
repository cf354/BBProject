// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BBGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/BBPlayerController.h"
#include "Player/BBPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Game/BBGameModeBase.h"


ABBGameStateBase::ABBGameStateBase()
	:TurnMaxTime(10)
{
	TurnRemainTime = TurnMaxTime;
	bReplicates = true;
}

void ABBGameStateBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TurnRemainTime);
}

void ABBGameStateBase::StartCountDown()
{
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			TurnTimerHandle,
			this,
			&ABBGameStateBase::CountDown,
			1.0f,
			true
		);
	}
}

void ABBGameStateBase::ResetCountDown()
{
	if (HasAuthority())
	{
		TurnRemainTime = TurnMaxTime;
		MulticastRPCBroadcastTimer();
		GetWorldTimerManager().ClearTimer(TurnTimerHandle);
	}
}

void ABBGameStateBase::CountDown()
{
	ABBGameModeBase* BBGM = Cast<ABBGameModeBase>(GetWorld()->GetAuthGameMode());
	if (TurnRemainTime <= 0)
	{
		if (IsValid(BBGM))
		{
			BBGM->TurnOver();
		}
	}

	MulticastRPCBroadcastTimer();

	--TurnRemainTime;
}

	void ABBGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString & InNameString)
	{
		if (!HasAuthority())
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (IsValid(PC))
			{
				ABBPlayerController* BBPC = Cast<ABBPlayerController>(PC);
				if (IsValid(BBPC))
				{
					FString NotificationString = InNameString + TEXT(" has joined the game");
					BBPC->PrintChatMessageString(NotificationString);
				}
			}
		}
	}

	void ABBGameStateBase::MulticastRPCBroadcastStartMessage_Implementation()
	{
		if (!HasAuthority())
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (IsValid(PC))
			{
				ABBPlayerController* BBPC = Cast<ABBPlayerController>(PC);
				if (IsValid(BBPC))
				{
					ABBPlayerState* BBPS = BBPC->GetPlayerState<ABBPlayerState>();
					if (IsValid(BBPS))
					{
						BBPS->InMyTurn = false;
						BBPC->NotificationText = FText::FromString("Not Your Turn");
					}
				}
			}
		}
	}


	void ABBGameStateBase::MulticastRPCBroadcastTimer_Implementation()
	{
		
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			ABBPlayerController* BBPC = Cast<ABBPlayerController>(PC);
			if(BBPC)
			BBPC->UpdateUITimer(TurnRemainTime);
		}
		
	}



