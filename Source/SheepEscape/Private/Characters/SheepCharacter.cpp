// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SheepCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ASheepCharacter::ASheepCharacter()
{
	//Test : minus speed on sheep
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

void ASheepCharacter::Action(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("HeadButt"));
}
