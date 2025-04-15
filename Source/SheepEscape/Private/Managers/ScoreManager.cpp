// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/ScoreManager.h"

void UScoreManager::InitializeScoreManager()
{
	InitializeActionToScoreMap();
	InitializePlayerActions();
	InitializePlayerScores();
}

void UScoreManager::InitializeActionToScoreMap()
{
	ActionToScoreMap.Empty();

	ActionToScoreMap.Add(EScoreAction::ESA_SheepElimateSheep, 2);
	ActionToScoreMap.Add(EScoreAction::ESA_SheepElimateSheperd, 10);
	ActionToScoreMap.Add(EScoreAction::ESA_SheepSurvive, 4);
	ActionToScoreMap.Add(EScoreAction::ESA_ShepherdEliminatePlayerSheep, 4);
	ActionToScoreMap.Add(EScoreAction::ESA_ShepherdRescueSheep, 1);
}

void UScoreManager::InitializePlayerActions()
{
	PlayerActions.Empty();

	for (int32 i = 0; i < 4; i++)
	{
		PlayerActions.Add(i, TArray<EScoreAction>());
	}
}

void UScoreManager::InitializePlayerScores()
{
	PlayerScores.Empty();

	for (int32 i = 0; i < 4; i++)
	{
		PlayerScores.Add(0);
	}
}

void UScoreManager::AddAction(int32 PlayerID, EScoreAction Action)
{
	if (PlayerActions.Contains(PlayerID))
	{
		PlayerActions[PlayerID].Add(Action);
	}
}

void UScoreManager::AddScores()
{
	for (int32 i = 0; i < PlayerScores.Num(); i++)
	{
		for (EScoreAction Action : PlayerActions[i])
		{
			if (ActionToScoreMap.Contains(Action))
			{
				PlayerScores[i] += ActionToScoreMap[Action];
			}
		}
		PlayerActions[i].Empty();
	}
}

void UScoreManager::DisplayPlayerActions()
{
	for (int32 i = 0; i < PlayerActions.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player %d Actions:"), i);
		for (EScoreAction Action : PlayerActions[i])
		{
			UE_LOG(LogTemp, Warning, TEXT(" - %s"), *UEnum::GetValueAsString(Action));
		}
	}
}

void UScoreManager::DisplayPlayerScores()
{
	for (int32 i = 0; i < PlayerScores.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player %d Score: %d"), i, PlayerScores[i]);
	}
}
