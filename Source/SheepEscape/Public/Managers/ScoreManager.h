// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utils/ScoreAction.h"
#include "UObject/NoExportTypes.h"
#include "ScoreManager.generated.h"

UCLASS()
class SHEEPESCAPE_API UScoreManager : public UObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void InitializeScoreManager();

	// Map that link a EScoreAction to a score int
	void InitializeActionToScoreMap();
	TMap<EScoreAction, int32> ActionToScoreMap;

	// Map that stock the actions that have been done for each player
	UFUNCTION(BlueprintCallable)
	void InitializePlayerActions();
	TMap<int32, TArray<EScoreAction>> PlayerActions;

	// Array that stock the score of each player
	void InitializePlayerScores();
	TArray<int32> PlayerScores;

	/** Add Scores */
	UFUNCTION(BlueprintCallable)
	void AddAction(int32 PlayerID, EScoreAction Action);

	UFUNCTION(BlueprintCallable)
	void AddScores();

	UFUNCTION(BlueprintCallable)
	void RemoveActions(int32 PlayerID);

	/** Temporary Display */
	UFUNCTION(BlueprintCallable)
	void DisplayPlayerActions();
	UFUNCTION(BlueprintCallable)
	void DisplayPlayerScores();
};
