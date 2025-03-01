// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ShepherdCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AShepherdCharacter::AShepherdCharacter()
{
	//Test : plus speed on sheepherd
	GetCharacterMovement()->MaxWalkSpeed = 450.f;
}

void AShepherdCharacter::Action(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("CrookHit"));
}
