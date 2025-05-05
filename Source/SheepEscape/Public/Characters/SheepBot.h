// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Data/BoidData.h"
#include "SheepBot.generated.h"

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

	// Common parameters
	float MaxSpeed = 450.f;
	UPROPERTY(EditDefaultsOnly, Category = "Boids Common Parameters")
	float Acceleration = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "Boids Common Parameters")
	float Inertia = 0.995f;
	UPROPERTY(EditDefaultsOnly, Category = "Boids Common Parameters", meta = (ClampMin = "0.5", ClampMax = "0.9", UIMin = "0.5", UIMax = "0.9"))
	float EmotionalStateMultiplier = 0.7f;
	float EmotionalState = 0.f;

	/** Movements */
	void Move(float DeltaTime);
	void BoidMovement(float DeltaTime);

	/** Velocity Update */
	void UpdateVelocity(float DeltaTime);
	FVector Velocity = FVector::ZeroVector;

	FVector Cohesion();
	FVector Separation();
	FVector Alignment();
	FVector Escape();
	void EmotionalStateUpdate(float DeltaTime);
	float CloseToShepherdNormalize(float DistanceToShepherd);
	float CloseToMaxSpeedNormalize();

	/** Boid Parameters */
	UPROPERTY(EditDefaultsOnly, Category = "Boid Data")
	UBoidData* BoidData;
};
