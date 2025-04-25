// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SheepBot.h"
#include "Characters/SheepCharacter.h"
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

	if (Controller)
	{
		Move(DeltaTime);
	}
}

void ASheepBot::Eliminate()
{
	if (IsEliminate()) return;

	Super::Eliminate();

	if(PushedBy) PushedBy->AddEliminateSheepAction();

	GameInstance->SheepElimination(this);
	Destroy();
}

void ASheepBot::Move(float DeltaTime)
{
	FVector Velocity = Escape();
	Velocity.Normalize();
	GetCharacterMovement()->Velocity = Velocity * MaxSpeed;
}

FVector ASheepBot::Cohesion()
{
	return (GameInstance->HerdCenter - GetActorLocation()).GetSafeNormal();
}

FVector ASheepBot::Separation()
{
	FVector SeparationVelocity = FVector::ZeroVector;

	for (AActor* Sheep : GameInstance->Sheeps)
	{
		if (Sheep == this) continue;

		FVector Distance = GetActorLocation() - Sheep->GetActorLocation();
		SeparationVelocity += Distance.GetSafeNormal() * inv(Distance.Length(), 1);
	}

	return SeparationVelocity;
}

FVector ASheepBot::Alignment()
{
	FVector AlignmentVelocity = FVector::ZeroVector;

	for (AActor* Sheep : GameInstance->Sheeps)
	{
		if (Sheep == this) continue;

		FVector Distance = GetActorLocation() - Sheep->GetActorLocation();
		if (Distance.Length() <= AlignementRadius)
		{
			AlignmentVelocity += Sheep->GetVelocity();
		}
	}

	return AlignmentVelocity;
}

FVector ASheepBot::Escape()
{
	if (GameInstance->Shepherd)
	{
		FVector Distance = GetActorLocation() - GameInstance->Shepherd->GetActorLocation();
		return Distance.GetSafeNormal() * inv(Distance.Length(), 10);
	}

	return FVector::ZeroVector;
}

float ASheepBot::inv(float x, float s)
{
	float epsilon = 0.0001f;
	return FMath::Pow(x / s + epsilon, -2);
}
