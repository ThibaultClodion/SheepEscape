// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SheepBot.h"
#include "Characters/SheepCharacter.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
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
	BoidMovement(DeltaTime);
}

void ASheepBot::BoidMovement(float DeltaTime)
{
	UpdateVelocity(DeltaTime);

	if ((Velocity / MaxSpeed).Length() >= BoidData->MinVelocityToStopState || EmotionalState >= 0.08f)
	{
		AddMovementInput(Velocity / MaxSpeed);
	}
	// If Velocity is too slow start gazing
	else
	{
		GetCharacterMovement()->StopMovementImmediately();
		Velocity = FVector::ZeroVector;	// Don't let velocity increment
	}
}

void ASheepBot::UpdateVelocity(float DeltaTime)
{
	EmotionalStateUpdate(DeltaTime);

	FVector TargetVelocity;

	TargetVelocity = Velocity
		+ (BoidData->CohesionFactor + (BoidData->CohesionStressFactor - BoidData->CohesionFactor) * EmotionalState) * Cohesion()
		+ (BoidData->SeparationFactor + (BoidData->SeparationStressFactor - BoidData->SeparationFactor) * EmotionalState) * Separation()
		+ (BoidData->AlignmentFactor + (BoidData->AlignmentStressFactor - BoidData->AlignmentFactor) * EmotionalState) * Alignment()
		+ BoidData->EscapeFactor * EmotionalState  * Escape();


	Velocity = FMath::Lerp(Velocity, TargetVelocity, DeltaTime * Acceleration);
	Velocity *= Inertia;
	Velocity = Velocity.GetClampedToSize(0.f, MaxSpeed);
}

FVector ASheepBot::Cohesion()
{
	FVector AveragePosition = FVector::ZeroVector;
	int nbSheepInCohesionRadius = 0;

	for (AActor* Sheep : GameInstance->Sheeps)
	{
		if (Sheep == this) continue;

		if ((Sheep->GetActorLocation() - GetActorLocation()).Length() <= BoidData->CohesionRadius)
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
		if (Distance <= BoidData->SeparationRadius)
		{
			SeparationVelocity += (GetActorLocation() - Sheep->GetActorLocation()) * (BoidData->SeparationRadius - Distance) / Distance;
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

		if ((Sheep->GetActorLocation() - GetActorLocation()).Length() <= BoidData->AlignmentRadius)
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
		if (Distance <= BoidData->EscapeRadius)
		{
			return (GetActorLocation() - GameInstance->Shepherd->GetActorLocation()) / Distance;
		}
	}

	return FVector::ZeroVector;
}

void ASheepBot::EmotionalStateUpdate(float DeltaTime)
{
	/* Assign EmotionalState to 0.f if the shepherd is not in the EscapeRadius
	* Assign EmotionalState to ~1.f if the shepherd is near the sheep */

	if (GameInstance->Shepherd)
	{
		float DistanceToShepherd = (GetActorLocation() - GameInstance->Shepherd->GetActorLocation()).Length();
		if (DistanceToShepherd <= BoidData->EscapeRadius)
		{
			float TargetEmotionalState = FMath::Clamp(CloseToShepherdNormalize(DistanceToShepherd) + CloseToMaxSpeedNormalize() / 2.f, 0, 1.f);
			EmotionalState = FMath::Lerp(EmotionalState, CloseToShepherdNormalize(DistanceToShepherd), BoidData->EmotionalIncreaseLerpFactor * DeltaTime);
		}

		else
		{
			EmotionalState = FMath::Lerp(EmotionalState, CloseToMaxSpeedNormalize() / 2.f, BoidData->EmotionalIncreaseLerpFactor * DeltaTime);
		}
	}

	else
	{
		if ((Velocity / MaxSpeed).Length() >= BoidData->MinVelocityToStopState)
		{
			EmotionalState = FMath::Lerp(EmotionalState, CloseToMaxSpeedNormalize() / 2.f, BoidData->EmotionalIncreaseLerpFactor * DeltaTime);
		}
		else
		{
			EmotionalState = FMath::Lerp(EmotionalState, 0.f, BoidData->EmotionalDecreaseLerpFactor * DeltaTime * 5);
		}
	}
}

float ASheepBot::CloseToShepherdNormalize(float DistanceToShepherd)
{
	return DistanceToShepherd / BoidData->EscapeRadius;
}

float ASheepBot::CloseToMaxSpeedNormalize()
{
	return Velocity.Length() / MaxSpeed;
}
