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
	// If Velocity is too slow
	else
	{
		StartGazing();
	}
}

void ASheepBot::GazingMovement()
{
	// If Velocity is too high -> stop gazing to follow herd
	if ((Velocity / MaxSpeed).Length() >= MinVelocityToStopGaze)
	{
		StopGazing();
	}
	// Don't let velocity increment
	else
	{
		Velocity = FVector::ZeroVector;
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

void ASheepBot::StartGazing()
{
	IsGazing = true;

	// Stop movement
	Velocity = FVector::ZeroVector;
	GetCharacterMovement()->StopMovementImmediately();

	// Wait to gaze a bit before moving
	const float GazingTime = FMath::RandRange(MinWaitTime, MaxWaitTime);
	GetWorldTimerManager().SetTimer(WaitTimer, this, &ASheepBot::RandomMovement, GazingTime);
}

void ASheepBot::StopGazing()
{
	IsGazing = false;

	// Stop waiting
	GetWorldTimerManager().ClearTimer(WaitTimer);

	// Stop movement
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
		GetCharacterMovement()->StopMovementImmediately();
	}
}

void ASheepBot::RandomMovement()
{
	// Get NavSys
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return;

	bool OneChanceOutOfTwenty = (FMath::RandRange(0, 20) == 0);

	FVector2D Rand2D;
	// Big random movement
	if (OneChanceOutOfTwenty)
	{
		Rand2D = FMath::RandPointInCircle(1.f).GetSafeNormal() * FMath::RandRange(MinBigRandomMovementDistance, MaxBigRandomMovementDistance);
	}
	// Little random movement
	else
	{
		Rand2D = FMath::RandPointInCircle(1.f).GetSafeNormal() * FMath::RandRange(MinLittleRandomMovementDistance, MaxLittleRandomMovementDistance);
	}

	// Project point to navigation
	FVector RawTarget = GetActorLocation() + FVector(Rand2D.X, Rand2D.Y, 0.f);
	FNavLocation NavLocation;
	if (!NavSys->ProjectPointToNavigation(RawTarget, NavLocation)) return;

	// Get a path to the location
	UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), NavLocation);
	if (!NavPath || NavPath->PathPoints.Num() < 2) return;

	// Reset path points
	PathPoints.Empty();
	PathPoints = NavPath->PathPoints;
	PathPointsIndex = 0;

	// Move to the first point
	MoveToNextPointOnPath(0, 0);
}

void ASheepBot::MoveToNextPointOnPath(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (PathPointsIndex >= PathPoints.Num())
	{
		// Fin du chemin, on recommence à gazer
		StartGazing();
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("MoveToNextPointOnPath, index = %d"), PathPointsIndex);

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController) return;

	// Ajoute un petit offset aléatoire pour "humaniser"
	FVector RandomOffset = FVector(FMath::RandRange(-OffsetPointDistance, OffsetPointDistance), FMath::RandRange(-OffsetPointDistance, OffsetPointDistance), 0);
	FVector TargetLocation = PathPoints[PathPointsIndex] + RandomOffset;

	// Mets à jour l'index pour le prochain appel
	PathPointsIndex++;

	// Écoute uniquement si on n’est pas déjà lié (évite les empilements)
	if (auto* PathComp = AIController->GetPathFollowingComponent())
	{
		PathComp->OnRequestFinished.Clear();
		PathComp->OnRequestFinished.AddUObject(this, &ASheepBot::MoveToNextPointOnPath);
	}

	// Lance le mouvement
	AIController->MoveToLocation(TargetLocation, 5.f);
}
