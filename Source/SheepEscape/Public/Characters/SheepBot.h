// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "SheepBot.generated.h"

class USphereComponent;
class AShepherdCharacter;
struct FAIRequestID;
struct FPathFollowingResult;

UCLASS()
class SHEEPESCAPE_API ASheepBot : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ASheepBot();

	virtual void Tick(float DeltaTime) override;

	virtual void Eliminate() override;

protected:
	virtual void BeginPlay() override;

private:
	/** Movements */
	void Move(float DeltaTime);
	void BoidMovement();
	void GazingMovement();

	/** Velocity Update */
	void UpdateVelocity(float DeltaTime);
	FVector Cohesion();
	FVector Separation();
	FVector Alignment();
	FVector Escape();
	void EmotionalStateUpdate(float DeltaTime);
	FVector Velocity = FVector::ZeroVector;

	/** Boids Parameters */

	// Common parameters
	UPROPERTY(EditDefaultsOnly, Category = "Boids Common Parameters")
	float MaxSpeed = 450.f;
	UPROPERTY(EditDefaultsOnly, Category = "Boids Common Parameters")
	float Acceleration = 450.f;
	UPROPERTY(EditDefaultsOnly, Category = "Boids Common Parameters")
	float Inertia = 0.99f;
	UPROPERTY(EditDefaultsOnly, Category = "Boids Common Parameters")
	float MinVelocityToStopMove = 0.15f;
	UPROPERTY(EditDefaultsOnly, Category = "Boids Common Parameters", meta = (ClampMin = "0.5", ClampMax = "0.9", UIMin = "0.5", UIMax = "0.9"))
	float EmotionalStateMultiplier = 0.7f;

	// Cohesion parameters
	UPROPERTY(EditDefaultsOnly, Category = "Cohesion Parameters")
	float CohesionFactor = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Cohesion Parameters")
	float CohesionStressFactor = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Cohesion Parameters", meta = (ClampMin = "300.0", ClampMax = "700.0", UIMin = "300.0", UIMax = "700.0"))
	float CohesionRadius = 500.f;

	// Separation parameters
	UPROPERTY(EditDefaultsOnly, Category = "Separation Parameters")
	float SeparationFactor = 0.4f;
	UPROPERTY(EditDefaultsOnly, Category = "Separation Parameters")
	float SeparationStressFactor = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Separation Parameters", meta = (ClampMin = "100.0", ClampMax = "300.0", UIMin = "100.0", UIMax = "300.0"))
	float SeparationRadius = 200.f;

	// Alignment parameters
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters")
	float AlignmentFactor = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters")
	float AlignmentStressFactor = 0.15f;
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters", meta = (ClampMin = "100.0", ClampMax = "300.0", UIMin = "100.0", UIMax = "300.0"))
	float AlignmentRadius = 300.f;

	// Escape parameters
	UPROPERTY(EditDefaultsOnly, Category = "Escape Parameters")
	float EscapeFactor = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Escape Parameters", meta = (ClampMin = "360.0", ClampMax = "480.0", UIMin = "360.0", UIMax = "480.0"))
	float EscapeRadius = 420.f;
	UPROPERTY(EditDefaultsOnly, Category = "Escape Parameters")
	float EmotionalStateLerpFactor = 3.f;
	float EmotionalState = 0.f;

	/** Gazing Parameters */
	void StartGazing();
	void StopGazing();

	bool IsGazing = false;

	UPROPERTY(EditDefaultsOnly, Category = "Gazing Parameters")
	float MinVelocityToStopGaze = 0.15f;

	// Timer
	FTimerHandle WaitTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Gazing Parameters")
	float MinWaitTime = 3.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Gazing Parameters")
	float MaxWaitTime = 30.f;

	// Random movement
	void RandomMovement();
	void MoveToNextPointOnPath(FAIRequestID RequestID, const FPathFollowingResult& Result);
	TArray<FVector> PathPoints;
	int PathPointsIndex;
	UPROPERTY(EditDefaultsOnly, Category = "Gazing Parameters")
	float OffsetPointDistance = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Gazing Parameters")
	float MinLittleRandomMovementDistance = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Gazing Parameters")
	float MaxLittleRandomMovementDistance = 150.f;
	UPROPERTY(EditDefaultsOnly, Category = "Gazing Parameters")
	float MinBigRandomMovementDistance = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Gazing Parameters")
	float MaxBigRandomMovementDistance = 1000.f;

};
