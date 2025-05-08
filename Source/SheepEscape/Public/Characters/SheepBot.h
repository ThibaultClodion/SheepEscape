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

	// Elimination
	ASheepCharacter* GetNearestSheepCharacter();

	UPROPERTY(EditDefaultsOnly, Category = "Elimination Parameters")
	float MaxDistanceToEarnScore = 3000.f;

	// Parameters
	float MaxSpeed = 450.f;
	UPROPERTY(EditDefaultsOnly, Category = "Boids Common Parameters")
	float Acceleration = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "Boids Common Parameters")
	float Inertia = 0.995f;
	UPROPERTY(EditDefaultsOnly, Category = "Boid Data")
	UBoidData* BoidData;
	UPROPERTY(EditDefaultsOnly, Category = "Boids Common Parameters", meta = (ClampMin = "0.5", ClampMax = "0.9", UIMin = "0.5", UIMax = "0.9"))
	float EmotionalStateMultiplier = 0.7f;
	float EmotionalState = 0.f;


	/** Movements */
	void Move(float DeltaTime);
	void BoidMovement();
	void GrazeMovement();

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

	/** Grazing */
	void StartGrazing();
	void SetGrazingTimer(float min, float max);
	void StopGrazing();

	bool IsGrazing = false;
	FVector GrazeVelocity = FVector::ZeroVector;
	FTimerHandle GrazeTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MaxVelocityToStopGraze = 0.3f;

	// Wait
	void Wait();
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MinWaitTime = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MaxWaitTime = 10.f;

	// Walk
	void WalkToOtherGrazingSpot();
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MinWalkTime = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MaxWalkTime = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MinWalkVelocity = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MaxWalkVelocity = 0.7f;

	// Run
	void RunToOtherGrazingSpot();
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MinRunTime = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MaxRunTime = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MinRunVelocity = 0.7f;
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MaxRunVelocity = 1.f;
};
