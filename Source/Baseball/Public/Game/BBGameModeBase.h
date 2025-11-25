// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BBGameModeBase.generated.h"

class ABBPlayerController;

UCLASS()
class BASEBALL_API ABBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPostLogin(AController* NewPlayer) override;
	
	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	void PrintChatMessageString(ABBPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(ABBPlayerController* InChattingPlayerController);

	void ResetGame();

	void JudgeGame(ABBPlayerController* InChattingPlayerController, int InStrikeCount);

	void TurnOver();

	void CheckChatGameStart(ABBPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void EndGame();

protected:
	bool GameStart = false;

	ABBPlayerController* CurrentTurnController = nullptr;

	int32 CurrentTurnControllerNumber = -1;

	FString SecretNumberString;

	TArray<TObjectPtr<ABBPlayerController>> AllPlayerControllers;
};
