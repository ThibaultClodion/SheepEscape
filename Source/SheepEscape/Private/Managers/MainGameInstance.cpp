// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/MainGameInstance.h"

void UMainGameInstance::Init()
{
	Super::Init();
	ScoreManager = NewObject<UScoreManager>(this, UScoreManager::StaticClass());
}
