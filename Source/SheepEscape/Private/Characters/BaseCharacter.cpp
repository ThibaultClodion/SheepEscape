// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//Define default rotation mode
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::DisableCharacter()
{
	// If it's a player, disable his input
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		DisableInput(PlayerController);
	}

	// Disable the character movement, collision and visibility
	PrimaryActorTick.bCanEverTick = false;
	GetCharacterMovement()->StopMovementImmediately();
	GetCapsuleComponent()->DestroyComponent();
	SetActorHiddenInGame(true);
}

bool ABaseCharacter::IsEliminate()
{
	return IsHidden();
}

void ABaseCharacter::Pushed(ASheepCharacter* Pusher, FVector Direction, float HeadbuttForceScale)
{
	// Save the pusher
	InterruptPushTimer();
	StartPushTimer(Pusher);

	GetCharacterMovement()->AddImpulse(Direction.GetSafeNormal() * HeadbuttForceScale);
}

void ABaseCharacter::StartPushTimer(ASheepCharacter* Pusher)
{
	PushedBy = Pusher;
	GetWorldTimerManager().SetTimer(PushedTimer, this, &ABaseCharacter::StopPushTimer, PushedTime);
}

void ABaseCharacter::StopPushTimer()
{
	PushedBy = NULL;
}

void ABaseCharacter::InterruptPushTimer()
{
	StopPushTimer();
	GetWorldTimerManager().ClearTimer(PushedTimer);
}