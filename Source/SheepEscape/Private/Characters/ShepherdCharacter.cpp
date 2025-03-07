// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ShepherdCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AShepherdCharacter::AShepherdCharacter()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
}

void AShepherdCharacter::Action(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("CrookHit"));
}

void AShepherdCharacter::Eliminate()
{
	UE_LOG(LogTemp, Warning, TEXT("ShepherdCharacter Eliminated !"));
}
