// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SheepCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ASheepCharacter::ASheepCharacter()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
}

void ASheepCharacter::Action(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("HeadButt"));
}

void ASheepCharacter::Eliminate()
{
	UE_LOG(LogTemp, Warning, TEXT("SheepCharacter Eliminated !"));
}
