// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/MainGameInstance.h"
#include "Characters/SheepCharacter.h"
#include "Characters/SheepBot.h"
#include <Kismet/GameplayStatics.h>

void UMainGameInstance::Init()
{
	Super::Init();
	ScoreManager = NewObject<UScoreManager>(this, UScoreManager::StaticClass());
}

void UMainGameInstance::RemoveScore(int32 PlayerID)
{
	ScoreManager->RemoveActions(PlayerID);
}

void UMainGameInstance::InitializeSheeps()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASheepBot::StaticClass(), Sheeps);

	TArray<AActor*> SheepCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASheepCharacter::StaticClass(), SheepCharacters);
	Sheeps.Append(SheepCharacters);
}
