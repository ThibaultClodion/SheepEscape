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
	UpdateVelocity(DeltaTime, NormalBoidData);

	if ((Velocity / MaxSpeed).Length() >= NormalBoidData->MinVelocityToStopState)
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

void ASheepBot::UpdateVelocity(float DeltaTime, UBoidData* BoidData)
{
	EmotionalStateUpdate(DeltaTime, BoidData);

	FVector TargetVelocity;

	TargetVelocity = Velocity
		+ (BoidData->CohesionFactor + (BoidData->CohesionStressFactor - BoidData->CohesionFactor) * EmotionalState) * Cohesion(BoidData)
		+ (BoidData->SeparationFactor + (BoidData->SeparationStressFactor - BoidData->SeparationFactor) * EmotionalState) * Separation(BoidData)
		+ (BoidData->AlignmentFactor + (BoidData->AlignmentStressFactor - BoidData->AlignmentFactor) * EmotionalState) * Alignment(BoidData)
		+ EmotionalState * BoidData->EscapeFactor * Escape(BoidData);


	Velocity = FMath::Lerp(Velocity, TargetVelocity, DeltaTime * Acceleration);
	Velocity *= Inertia;
	Velocity = Velocity.GetClampedToSize(0.f, MaxSpeed);
}

FVector ASheepBot::Cohesion(UBoidData* BoidData)
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

FVector ASheepBot::Separation(UBoidData* BoidData)
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

FVector ASheepBot::Alignment(UBoidData* BoidData)
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

FVector ASheepBot::Escape(UBoidData* BoidData)
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

void ASheepBot::EmotionalStateUpdate(float DeltaTime, UBoidData* BoidData)
{
	/* Assign EmotionalState to 0.f if the shepherd is not in the EscapeRadius
	* Assign EmotionalState to ~1.f if the shepherd is near the sheep */

	if (GameInstance->Shepherd)
	{
		float DistanceToShepherd = (GetActorLocation() - GameInstance->Shepherd->GetActorLocation()).Length();
		if (DistanceToShepherd <= BoidData->EscapeRadius)
		{
			EmotionalState = FMath::Lerp(EmotionalState, DistanceToShepherd / BoidData->EscapeRadius, BoidData->EmotionalStateLerpFactor * DeltaTime);
		}
		else
		{
			EmotionalState = FMath::Lerp(EmotionalState, 0.f, BoidData->EmotionalStateLerpFactor * DeltaTime);
		}
	}
	else
	{
		EmotionalState = 0.f;
	}
}