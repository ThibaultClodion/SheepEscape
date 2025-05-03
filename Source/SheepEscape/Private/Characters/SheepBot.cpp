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
	UpdateVelocity(DeltaTime);

	if (!IsGrazing)
	{
		BoidMovement();
	}
	else
	{
		GrazingMovement();
	}
}

void ASheepBot::BoidMovement()
{
	if ((Velocity / MaxSpeed).Length() >= Data->MinVelocityToStopMove)
	{
		AddMovementInput(Velocity / MaxSpeed);
	}
	// If Velocity is too slow start gazing
	else
	{
		StartGraze();
	}
}

void ASheepBot::GrazingMovement()
{
	// If Velocity is too high -> stop gazing to follow herd
	if ((Velocity / MaxSpeed).Length() >= FMath::Max(Data->MinVelocityToStopGraze, GrazeVelocity.Length()))
	{
		StopGraze();
	}
	else
	{
		AddMovementInput(GrazeVelocity);
		Velocity = FVector::ZeroVector;	// Don't let velocity increment
	}
}

void ASheepBot::UpdateVelocity(float DeltaTime)
{
	EmotionalStateUpdate(DeltaTime);

	FVector TargetVelocity;
	if (!IsGrazing)
	{
		TargetVelocity = Velocity
			+ (Data->CohesionFactor + (Data->CohesionStressFactor - Data->CohesionFactor) * EmotionalState) * Cohesion()
			+ (Data->SeparationFactor + (Data->SeparationStressFactor - Data->SeparationFactor) * EmotionalState) * Separation()
			+ (Data->AlignmentFactor + (Data->AlignmentStressFactor - Data->AlignmentFactor) * EmotionalState) * Alignment()
			+ EmotionalState * Data->EscapeFactor * Escape();
	}
	else
	{
		TargetVelocity = Velocity
			+ (Data->GrazeCohesionFactor + (Data->CohesionStressFactor - Data->GrazeCohesionFactor) * EmotionalState) * Cohesion()
			+ (Data->GrazeSeparationFactor + (Data->SeparationStressFactor - Data->GrazeSeparationFactor) * EmotionalState) * Separation()
			+ (Data->GrazeAlignmentFactor + (Data->AlignmentStressFactor - Data->GrazeAlignmentFactor) * EmotionalState) * Alignment()
			+ EmotionalState * Data->EscapeFactor * Escape();
	}


	Velocity = FMath::Lerp(Velocity, TargetVelocity, DeltaTime * Data->Acceleration);
	Velocity *= Data->Inertia;
	Velocity = Velocity.GetClampedToSize(0.f, MaxSpeed);
}

FVector ASheepBot::Cohesion()
{
	FVector AveragePosition = FVector::ZeroVector;
	int nbSheepInCohesionRadius = 0;

	for (AActor* Sheep : GameInstance->Sheeps)
	{
		if (Sheep == this) continue;

		if ((Sheep->GetActorLocation() - GetActorLocation()).Length() <= Data->CohesionRadius)
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
		if (Distance <= Data->SeparationRadius)
		{
			SeparationVelocity += (GetActorLocation() - Sheep->GetActorLocation()) * (Data->SeparationRadius - Distance) / Distance;
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

		if ((Sheep->GetActorLocation() - GetActorLocation()).Length() <= Data->AlignmentRadius)
		{
			nbSheepsInAlignmentRadius++;
			AlignmentVelocity += Sheep->GetVelocity();
		}
	}

	if (nbSheepsInAlignmentRadius == 0) return FVector::ZeroVector;

	if (IsGrazing)
	{
		return AlignmentVelocity / nbSheepsInAlignmentRadius;
	}
	else
	{
		return AlignmentVelocity / nbSheepsInAlignmentRadius - GetVelocity();
	}

}

FVector ASheepBot::Escape()
{
	if (GameInstance->Shepherd)
	{
		float Distance = (GetActorLocation() - GameInstance->Shepherd->GetActorLocation()).Length();
		if (Distance <= Data->EscapeRadius)
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
		if (DistanceToShepherd <= Data->EscapeRadius)
		{
			EmotionalState = FMath::Lerp(EmotionalState, DistanceToShepherd / Data->EscapeRadius, Data->EmotionalStateLerpFactor * DeltaTime);
		}
		else
		{
			EmotionalState = FMath::Lerp(EmotionalState, 0.f, Data->EmotionalStateLerpFactor * DeltaTime);
		}
	}
	else
	{
		EmotionalState = 0.f;
	}
}

void ASheepBot::StartGraze()
{
	// If he wasn't already gazing -> make a pause
	if (!IsGrazing)
	{
		IsGrazing = true;
		Velocity = FVector::ZeroVector;
		GetCharacterMovement()->StopMovementImmediately();

		GrazeVelocity = FVector::ZeroVector;

		SetGrazeTimer(Data->MinWaitTime, Data->MaxWaitTime);
	}
	else
	{
		int Random0To15 = FMath::RandRange(0, 15);

		// Wait
		if (Random0To15 <= 10)
		{
			GrazeVelocity = FVector::ZeroVector;

			SetGrazeTimer(Data->MinWaitTime, Data->MaxWaitTime);
		}

		// Little Movement to another graze position
		else if (Random0To15 <= 14)
		{
			FVector2D RandomPoint = FMath::RandPointInCircle(1.f).GetSafeNormal() * FMath::RandRange(Data->MinLittleMovementMagnitude, Data->MaxLittleMovementMagnitude);
			GrazeVelocity = FVector(RandomPoint.X, RandomPoint.Y, 0.f);

			SetGrazeTimer(Data->MinLittleMovementTime, Data->MaxLittleMovementTime);
		}

		// Big Movement to another graze position
		else if (Random0To15 == 15)
		{
			FVector2D RandomPoint = FMath::RandPointInCircle(1.f).GetSafeNormal() * FMath::RandRange(Data->MinBigMovementMagnitude, Data->MaxBigMovementMagnitude);
			GrazeVelocity = FVector(RandomPoint.X, RandomPoint.Y, 0.f);

			SetGrazeTimer(Data->MinBigMovementTime, Data->MaxBigMovementTime);
		}
	}
}

void ASheepBot::SetGrazeTimer(float min, float max)
{
	const float GazingTime = FMath::RandRange(min, max);
	GetWorldTimerManager().SetTimer(GrazeTimer, this, &ASheepBot::StartGraze, GazingTime);
}

void ASheepBot::StopGraze()
{
	IsGrazing = false;
	GetWorldTimerManager().ClearTimer(GrazeTimer);
}
