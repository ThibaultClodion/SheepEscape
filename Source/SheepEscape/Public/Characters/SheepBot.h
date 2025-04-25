// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "SheepBot.generated.h"

class USphereComponent;
class AShepherdCharacter;

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
	/** Boids Movement */
	void Move(float DeltaTime);
	FVector Cohesion();
	FVector Separation();
	FVector Alignment();
	FVector Escape();

	/** Boids Parameters */
	float Inv(float x, float s);
	float EmotionalState();

	// Common parameters
	UPROPERTY(EditDefaultsOnly, Category = "Boids Common Parameters")
	float MaxSpeed = 450.f;
	UPROPERTY(EditDefaultsOnly, Category = "Boids Common Parameters", meta = (ClampMin = "0.5", ClampMax = "0.9", UIMin = "0.5", UIMax = "0.9"))
	float EmotionalStateMultiplier = 0.7f;

	// Cohesion parameters
	UPROPERTY(EditDefaultsOnly, Category = "Cohesion Parameters", meta = (ClampMin = "1.0", ClampMax = "3.0", UIMin = "1.0", UIMax = "3.0"))
	float CohesionFactor = 2.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Cohesion Parameters", meta = (ClampMin = "5.0", ClampMax = "15.0", UIMin = "5.0", UIMax = "15.0"))
	float CohesionStressFactor = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Cohesion Parameters", meta = (ClampMin = "300.0", ClampMax = "700.0", UIMin = "300.0", UIMax = "700.0"))
	float CohesionRadius = 500.f;

	// Separation parameters
	UPROPERTY(EditDefaultsOnly, Category = "Separation Parameters", meta = (ClampMin = "0.14", ClampMax = "0.20", UIMin = "0.14", UIMax = "0.20"))
	float SeparationFactor = 0.17f;
	UPROPERTY(EditDefaultsOnly, Category = "Separation Parameters", meta = (ClampMin = "0.0", ClampMax = "0.0", UIMin = "0.0", UIMax = "0.0"))
	float SeparationStressFactor = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Separation Parameters", meta = (ClampMin = "100.0", ClampMax = "300.0", UIMin = "100.0", UIMax = "300.0"))
	float SeparationRadius = 200.f;

	// Alignment parameters
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters", meta = (ClampMin = "0.2", ClampMax = "0.8", UIMin = "0.2", UIMax = "0.8"))
	float AlignmentFactor = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters", meta = (ClampMin = "0.1", ClampMax = "0.3", UIMin = "0.1", UIMax = "0.3"))
	float AlignmentStressFactor = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters", meta = (ClampMin = "100.0", ClampMax = "300.0", UIMin = "100.0", UIMax = "300.0"))
	float AlignmentRadius = 300.f;

	// Escape parameters
	UPROPERTY(EditDefaultsOnly, Category = "Escape Parameters", meta = (ClampMin = "2.0", ClampMax = "8.0", UIMin = "2.0", UIMax = "8.0"))
	float EscapeFactor = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Escape Parameters", meta = (ClampMin = "360.0", ClampMax = "480.0", UIMin = "360.0", UIMax = "480.0"))
	float EscapeRadius = 420.f;
};
