// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SheepBot.h"
#include "Characters/SheepCharacter.h"
#include "Managers/MainGameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

ASheepBot::ASheepBot()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
}

void ASheepBot::BeginPlay()
{
	Super::BeginPlay();
}

void ASheepBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASheepBot::Eliminate()
{
	if (IsEliminate()) return;

	Super::Eliminate();

	if(PushedBy) PushedBy->AddEliminateSheepAction();

	UMainGameInstance* GameInstance = Cast<UMainGameInstance>(GetGameInstance());
	GameInstance->SheepElimination();

	Destroy();
}