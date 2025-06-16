#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SheepBotData.generated.h"

UCLASS(BlueprintType)
class SHEEPESCAPE_API USheepBotData : public UDataAsset
{
	GENERATED_BODY()

public:

	// Speed Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Speed Parameters")
	float Acceleration = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "Speed Parameters")
	float Deceleration = 4.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Speed Parameters")
	float Inertia = 0.995f;
	UPROPERTY(EditDefaultsOnly, Category = "Speed Parameters")
	float MinDecelerationVelocity = 0.4;
	UPROPERTY(EditDefaultsOnly, Category = "Speed Parameters")
	float MinVelocity = 0.1f;


	// Elimination Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Elimination Parameters")
	float MaxDistanceToEarnScore = 800.f;

	// Cohesion Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Cohesion Parameters")
	float CohesionFactor = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Cohesion Parameters")
	float CohesionStressFactor = 0.4f;
	UPROPERTY(EditDefaultsOnly, Category = "Cohesion Parameters")
	float CohesionRadius = 500.f;

	// Separation Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Separation Parameters")
	float SeparationFactor = 0.4f;
	UPROPERTY(EditDefaultsOnly, Category = "Separation Parameters")
	float SeparationStressFactor = 0.05f;
	UPROPERTY(EditDefaultsOnly, Category = "Separation Parameters")
	float SeparationRadius = 340.f;

	// Alignment Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters")
	float AlignmentFactor = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters")
	float AlignmentStressFactor = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters")
	float AlignmentRadius = 300.f;

	// Escape Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Escape Parameters")
	float EscapeFactor = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Escape Parameters")
	float EscapeRadius = 480.f;

	// Emotional Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Emotional Parameters")
	float EmotionalIncreaseFactor = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Emotional Parameters")
	float EmotionalDecreaseFactor = 10.f;
};