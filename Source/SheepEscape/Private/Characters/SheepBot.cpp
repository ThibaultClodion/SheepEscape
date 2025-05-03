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

	if (!IsGazing)
	{
		BoidMovement();
	}
	else
	{
		GazingMovement();
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
		StartGaze();
	}
}

void ASheepBot::GazingMovement()
{
	// If Velocity is too high -> stop gazing to follow herd
	if ((Velocity / MaxSpeed).Length() >= FMath::Max(Data->MinVelocityToStopGaze, GazeVelocity.Length()))
	{
		StopGaze();
	}
	else
	{
		AddMovementInput(GazeVelocity);
		Velocity = FVector::ZeroVector;	// Don't let velocity increment
	}
}

void ASheepBot::UpdateVelocity(float DeltaTime)
{
	EmotionalStateUpdate(DeltaTime);

	FVector TargetVelocity;
	if (!IsGazing)
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
			+ (Data->GazeCohesionFactor + (Data->CohesionStressFactor - Data->GazeCohesionFactor) * EmotionalState) * Cohesion()
			+ (Data->GazeSeparationFactor + (Data->SeparationStressFactor - Data->GazeSeparationFactor) * EmotionalState) * Separation()
			+ (Data->GazeAlignmentFactor + (Data->AlignmentStressFactor - Data->GazeAlignmentFactor) * EmotionalState) * Alignment()
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

	if (IsGazing)
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

		/*Previous function with arctan to smooth the curve but not really useful
		return 1 / 3.14 * FMath::Atan((EscapeRadius - DistanceToShepherd) / EmotionalStateMultiplier) + 0.5f; */
	}
	else
	{
		EmotionalState = 0.f;
	}
}

void ASheepBot::StartGaze()
{
	// If the sheep is first time gazing make a pause
	if (!IsGazing)
	{
		IsGazing = true;
		Velocity = FVector::ZeroVector;
		GetCharacterMovement()->StopMovementImmediately();

		// Pause
		GazeVelocity = FVector::ZeroVector;

		const float GazingTime = FMath::RandRange(Data->MinWaitTime, Data->MaxWaitTime);
		GetWorldTimerManager().SetTimer(GazeTimer, this, &ASheepBot::StartGaze, GazingTime);
	}
	else
	{
		int Random0To5 = FMath::RandRange(0, 15);
		if (Random0To5 <= 10)
		{
			// Pause
			GazeVelocity = FVector::ZeroVector;

			const float GazingTime = FMath::RandRange(Data->MinWaitTime, Data->MaxWaitTime);
			GetWorldTimerManager().SetTimer(GazeTimer, this, &ASheepBot::StartGaze, GazingTime);
		}
		else if (Random0To5 <= 14)
		{
			// Little Movement
			FVector2D RandomPoint = FMath::RandPointInCircle(1.f).GetSafeNormal() * FMath::RandRange(Data->MinLittleMovementMagnitude, Data->MaxLittleMovementMagnitude);
			GazeVelocity = FVector(RandomPoint.X, RandomPoint.Y, 0.f);

			const float GazingTime = FMath::RandRange(Data->MinLittleMovementTime, Data->MaxLittleMovementTime);
			GetWorldTimerManager().SetTimer(GazeTimer, this, &ASheepBot::StartGaze, GazingTime);
		}
		else if (Random0To5 == 15)
		{
			// Big Movement
			FVector2D RandomPoint = FMath::RandPointInCircle(1.f).GetSafeNormal() * FMath::RandRange(Data->MinBigMovementMagnitude, Data->MaxBigMovementMagnitude);
			GazeVelocity = FVector(RandomPoint.X, RandomPoint.Y, 0.f);

			const float GazingTime = FMath::RandRange(Data->MinBigMovementTime, Data->MaxBigMovementTime);
			GetWorldTimerManager().SetTimer(GazeTimer, this, &ASheepBot::StartGaze, GazingTime);
		}
	}
}

void ASheepBot::StopGaze()
{
	IsGazing = false;
	GetWorldTimerManager().ClearTimer(GazeTimer);
}
