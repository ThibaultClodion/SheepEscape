// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BasePlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void ABasePlayer::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		InitializeMappingContext(PlayerController);
	}
}

void ABasePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasePlayer::Move);
		EnhancedInputComponent->BindAction(BaseAction, ETriggerEvent::Started, this, &ABasePlayer::Action);
	}
}

void ABasePlayer::Move(const FInputActionValue& Value)
{
	const FVector2D MovementValue = Value.Get<FVector2D>();

	if (Controller)
	{
		AddMovementInput(FVector(MovementValue.X, MovementValue.Y, 0.f));
	}
}

void ABasePlayer::Action(const FInputActionValue& Value) {}

void ABasePlayer::InitializeMappingContext(APlayerController* PlayerController)
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(CharacterMappingContext, 0);
	}
}
