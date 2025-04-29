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
	if ((Velocity / MaxSpeed).Length() >= MinVelocityToStopMove)
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
	if ((Velocity / MaxSpeed).Length() >= MinVelocityToStopGaze)
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

	FVector TargetVelocity = Velocity
		+ (CohesionFactor + (CohesionStressFactor - CohesionFactor) * EmotionalState) * Cohesion()
		+ (SeparationFactor + (SeparationStressFactor - SeparationFactor) * EmotionalState) * Separation()
		+ (AlignmentFactor + (AlignmentStressFactor - AlignmentFactor) * EmotionalState) * Alignment()
		+ EmotionalState * EscapeFactor * Escape();

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

void ASheepBot::EmotionalStateUpdate(float DeltaTime)
{
	/* Assign EmotionalState to 0.f if the shepherd is not in the EscapeRadius
	* Assign EmotionalState to ~1.f if the shepherd is near the sheep */

	if (GameInstance->Shepherd)
	{
		float DistanceToShepherd = (GetActorLocation() - GameInstance->Shepherd->GetActorLocation()).Length();
		if (DistanceToShepherd <= EscapeRadius)
		{
			EmotionalState = FMath::Lerp(EmotionalState, DistanceToShepherd / EscapeRadius, EmotionalStateLerpFactor * DeltaTime);
		}
		else
		{
			EmotionalState = FMath::Lerp(EmotionalState, 0.f, EmotionalStateLerpFactor * DeltaTime);
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
	if (!IsGazing)
	{
		IsGazing = true;
		Velocity = FVector::ZeroVector;
		GetCharacterMovement()->StopMovementImmediately();
	}

	int Random0To20 = FMath::RandRange(0, 20);
	if (Random0To20 < 10)
	{
		UE_LOG(LogTemp, Warning, TEXT("SheepBot is pause"));
		// Pause
		GazeVelocity = FVector::ZeroVector;

		const float GazingTime = FMath::RandRange(MinWaitTime, MaxWaitTime);
		GetWorldTimerManager().SetTimer(GazeTimer, this, &ASheepBot::StopGaze, GazingTime);
	}
	else if (Random0To20 < 15)
	{
		UE_LOG(LogTemp, Warning, TEXT("SheepBot is Little Movement"));
		// Little Movement
		FVector2D RandomPoint = FMath::RandPointInCircle(1.f).GetSafeNormal() * FMath::RandRange(MinLittleMovementMagnitude, MaxLittleMovementMagnitude);
		GazeVelocity = FVector(RandomPoint.X, RandomPoint.Y, 0.f);

		const float GazingTime = FMath::RandRange(MinLittleMovementTime, MaxLittleMovementTime);
		GetWorldTimerManager().SetTimer(GazeTimer, this, &ASheepBot::StopGaze, GazingTime);
	}
	else if (Random0To20 < 20)
	{
		UE_LOG(LogTemp, Warning, TEXT("SheepBot is Big Movement"));
		// Big Movement
		FVector2D RandomPoint = FMath::RandPointInCircle(1.f).GetSafeNormal() * FMath::RandRange(MinBigMovementMagnitude, MaxBigMovementMagnitude);
		GazeVelocity = FVector(RandomPoint.X, RandomPoint.Y, 0.f);

		const float GazingTime = FMath::RandRange(MinBigMovementTime, MaxBigMovementTime);
		GetWorldTimerManager().SetTimer(GazeTimer, this, &ASheepBot::StartGaze, GazingTime);
	}
}

void ASheepBot::StopGaze()
{
	IsGazing = false;
	GetWorldTimerManager().ClearTimer(GazeTimer);
}
