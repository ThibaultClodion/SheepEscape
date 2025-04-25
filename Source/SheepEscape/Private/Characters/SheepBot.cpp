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
	FVector Velocity = 
		CohesionFactor * (1 + EmotionalState() * CohesionStressFactor) * Cohesion()
		+ SeparationFactor * (1 + EmotionalState() * SeparationStressFactor) * Separation()
		+ AlignmentFactor * (1 + EmotionalState() * AlignmentStressFactor) * Alignment()
		+ EmotionalState() * EscapeFactor * Escape();

	AddMovementInput(Velocity);

	/*FVector VelocityMax = EmotionalState() * Velocity.GetSafeNormal();

	if (Velocity.Length() <= VelocityMax.Length())
	{
		AddMovementInput(Velocity / MaxSpeed);
	}
	else
	{
		AddMovementInput(VelocityMax / MaxSpeed);
	}*/
}

FVector ASheepBot::Cohesion()
{
	FVector AveragePosition = FVector::ZeroVector;
	int nbSheepInCohesionRadius = 0;

	for (AActor* Sheep : GameInstance->Sheeps)
	{
		if (Sheep == this) continue;

		if ((Sheep->GetActorLocation() - GetActorLocation()).Length() <= CohesionRadius)
		{
			nbSheepInCohesionRadius++;
			AveragePosition += Sheep->GetActorLocation();
		}
	}

	if (nbSheepInCohesionRadius == 0) return FVector::ZeroVector;

	return AveragePosition / nbSheepInCohesionRadius - GetActorLocation();
}

FVector ASheepBot::Separation()
{
	FVector SeparationVelocity = FVector::ZeroVector;

	for (AActor* Sheep : GameInstance->Sheeps)
	{
		if (Sheep == this) continue;

		float Distance = (GetActorLocation() - Sheep->GetActorLocation()).Length();
		if (Distance <=  SeparationRadius)
		{
			SeparationVelocity += (GetActorLocation() - Sheep->GetActorLocation()) * (SeparationRadius - Distance) / Distance;
		}
	}

	return SeparationVelocity;
}

FVector ASheepBot::Alignment()
{
	FVector AlignmentVelocity = FVector::ZeroVector;
	int nbSheepsInAlignmentRadius = 0;

	for (AActor* Sheep : GameInstance->Sheeps)
	{
		if (Sheep == this) continue;

		if ((Sheep->GetActorLocation() - GetActorLocation()).Length() <= AlignmentRadius)
		{
			nbSheepsInAlignmentRadius++;
			AlignmentVelocity += Sheep->GetVelocity();
		}
	}

	if (nbSheepsInAlignmentRadius == 0) return FVector::ZeroVector;

	return AlignmentVelocity / nbSheepsInAlignmentRadius - GetVelocity();
}

FVector ASheepBot::Escape()
{
	if (GameInstance->Shepherd)
	{
		float Distance = (GetActorLocation() - GameInstance->Shepherd->GetActorLocation()).Length();
		if (Distance <= EscapeRadius)
		{
			return (GetActorLocation() - GameInstance->Shepherd->GetActorLocation()) / Distance;
		}
	}

	return FVector::ZeroVector;
}

float ASheepBot::Inv(float x, float s)
{
	float epsilon = 0.0001f;
	return FMath::Pow(x / s + epsilon, -2);
}

float ASheepBot::EmotionalState()
{
	if (GameInstance->Shepherd)
	{
		float DistanceToShepherd = (GetActorLocation() - GameInstance->Shepherd->GetActorLocation()).Length();
		return 1 / 3.14 * FMath::Atan((EscapeRadius - DistanceToShepherd) / EmotionalStateMultiplier) + 0.5f;
	}

	return 0.f;
}
