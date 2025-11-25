// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BBGameStateBase.generated.h"


UCLASS()
class BASEBALL_API ABBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

	ABBGameStateBase();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastStartMessage();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastTimer();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void StartCountDown();

	void ResetCountDown();

	void CountDown();
	
private:
	FTimerHandle TurnTimerHandle;

	int32 TurnMaxTime;

	UPROPERTY(Replicated)
	int32 TurnRemainTime;
};
