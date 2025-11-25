// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BBGameModeBase.h"

#include "Game/BBGameStateBase.h"
#include "EngineUtils.h"
#include "Player/BBPlayerController.h"
#include "Player/BBPlayerState.h"


void ABBGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ABBPlayerController* BBPlayerController = Cast<ABBPlayerController>(NewPlayer);
	if (IsValid(BBPlayerController) == true)
	{
		BBPlayerController->NotificationText = FText::FromString(TEXT("Type 'Start' to begin"));

		AllPlayerControllers.Add(BBPlayerController);

		ABBPlayerState* BBPS = BBPlayerController->GetPlayerState<ABBPlayerState>();
		if (IsValid(BBPS) == true)
		{
			BBPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ABBGameStateBase* BBGameStateBase = GetGameState<ABBGameStateBase>();
		if (IsValid(BBGameStateBase) == true)
		{
			BBGameStateBase->MulticastRPCBroadcastLoginMessage(BBPS->PlayerNameString);
		}
	}
}

FString ABBGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ABBGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (UniqueDigits.Num() != 3)
		{
			bIsUnique = false;
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

FString ABBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ABBGameModeBase::PrintChatMessageString(ABBPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	FString ChatMessageString = InChatMessageString;
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);

	UE_LOG(LogTemp, Warning, TEXT("ABBGameModeBase::PrintChatMessageString who's Turn? %d"), CurrentTurnControllerNumber);

	ABBPlayerState* BBPS = InChattingPlayerController->GetPlayerState<ABBPlayerState>();
	if (!BBPS)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABBGameModeBase::PrintChatMessageString Error"));
	}

	if (IsGuessNumberString(GuessNumberString) == true && BBPS->InMyTurn && GameStart)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

		TurnOver();

		for (TActorIterator<ABBPlayerController> It(GetWorld()); It; ++It)
		{
			ABBPlayerController* BBPlayerController = *It;
			if (IsValid(BBPlayerController) == true)
			{
				FString CombinedMessageString = BBPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString + TEXT(" -> ") + JudgeResultString;
				UE_LOG(LogTemp, Error, TEXT("%s"), *BBPS->PlayerNameString);
				BBPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}

		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));		
		JudgeGame(InChattingPlayerController, StrikeCount);	
	}
	else
	{
		CheckChatGameStart(InChattingPlayerController,InChatMessageString);
		for (TActorIterator<ABBPlayerController> It(GetWorld()); It; ++It)
		{
			ABBPlayerController* BBPlayerController = *It;
			if (IsValid(BBPlayerController) == true)
			{
				BBPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

void ABBGameModeBase::IncreaseGuessCount(ABBPlayerController* InChattingPlayerController)
{
	ABBPlayerState* BBPS = InChattingPlayerController->GetPlayerState<ABBPlayerState>();

	if (IsValid(BBPS) == true)
	{
		BBPS->CurrentGuessCount++;
	}
}

void ABBGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	ABBGameStateBase* BBGS = GetGameState<ABBGameStateBase>();
	BBGS->MulticastRPCBroadcastStartMessage();

	BBGS->StartCountDown();

	for (const auto& BBPlayerController : AllPlayerControllers)
	{
		ABBPlayerState* BBPS = BBPlayerController->GetPlayerState<ABBPlayerState>();
		if (IsValid(BBPS) == true)
		{
			BBPS->CurrentGuessCount = 0;
		}
	}

	CurrentTurnControllerNumber = FMath::RandRange(0, AllPlayerControllers.Num() - 1);
	if (AllPlayerControllers[CurrentTurnControllerNumber])
	{
		CurrentTurnController = AllPlayerControllers[CurrentTurnControllerNumber];
	}
	else
	{
		CurrentTurnController = AllPlayerControllers[0];
	}
	
	CurrentTurnController->NotificationText = FText::FromString("Your Turn");
}

void ABBGameModeBase::JudgeGame(ABBPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		ABBPlayerState* BBPS = InChattingPlayerController->GetPlayerState<ABBPlayerState>();
		for (const auto& BBPlayerController : AllPlayerControllers)
		{
			if (IsValid(BBPS) == true)
			{
				FString CombinedMessageString = BBPS->PlayerNameString + TEXT(" has won the game.");
				BBPlayerController->NotificationText = FText::FromString(CombinedMessageString);
			}
		}
		EndGame();
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& BBPlayerController : AllPlayerControllers)
		{
			ABBPlayerState* BBPS = BBPlayerController->GetPlayerState<ABBPlayerState>();
			if (IsValid(BBPS) == true)
			{
				if (BBPS->CurrentGuessCount < BBPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& CXPlayerController : AllPlayerControllers)
			{
				CXPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));
			}
			EndGame();
		}
	}
}

void ABBGameModeBase::TurnOver()
{
	if (!CurrentTurnController)
	{
		UE_LOG(LogTemp, Error, TEXT("TurnOverError"));
		return;
	}

	IncreaseGuessCount(CurrentTurnController);

	ABBPlayerState* EndBBPS = CurrentTurnController->GetPlayerState<ABBPlayerState>();
	if (IsValid(EndBBPS))
	{
		EndBBPS->InMyTurn = false;
		CurrentTurnController->NotificationText = FText::FromString("Not Your Turn");
	}

	CurrentTurnControllerNumber = ++CurrentTurnControllerNumber % AllPlayerControllers.Num();
	CurrentTurnController = AllPlayerControllers[CurrentTurnControllerNumber];

	ABBPlayerState* StartBBPS = CurrentTurnController->GetPlayerState<ABBPlayerState>();
	if (IsValid(StartBBPS))
	{
		StartBBPS->InMyTurn = true;
		CurrentTurnController->NotificationText = FText::FromString("Your Turn");
	}

	ABBGameStateBase* BBGS = GetGameState<ABBGameStateBase>();
	if (BBGS)
	{
		BBGS->ResetCountDown();
		BBGS->StartCountDown();
	}
}

void ABBGameModeBase::CheckChatGameStart(ABBPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	if (InChatMessageString == FString("Start"))
	{
		GameStart = true;
		
		ResetGame();
	}
	else
	{
		return;
	}
}

void ABBGameModeBase::EndGame()
{	
	for (TActorIterator<ABBPlayerController> It(GetWorld()); It; ++It)
	{
		ABBPlayerController* BBPC = *It;
		FString NotificationString = BBPC->NotificationText.ToString() +TEXT("\nType 'Start' to begin");

		BBPC->NotificationText = FText::FromString(NotificationString);
	}
	ABBGameStateBase* BBGS = GetGameState<ABBGameStateBase>();
	if (IsValid(BBGS))
	{
		BBGS->ResetCountDown();
	}

	GameStart = false;
}
