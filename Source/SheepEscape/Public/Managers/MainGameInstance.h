// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Managers/ScoreManager.h"
#include "MainGameInstance.generated.h"

UCLASS()
class SHEEPESCAPE_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

	UPROPERTY(BlueprintReadOnly)
	UScoreManager* ScoreManager;

	/** Actions and Score */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void AddAction(APlayerController* PlayerController, EScoreAction Action);
	UFUNCTION(BlueprintImplementableEvent)
	void SheepElimination();
	UFUNCTION(BlueprintImplementableEvent)
	void ShepherdElimination();

	/** Game Data */
	UPROPERTY(BlueprintReadWrite)
	int NbSheeps;
};
