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
	UPROPERTY(EditDefaultsOnly, Category = "Cohesion Parameters", meta = (ClampMin = "300.0", ClampMax = "700.0", UIMin = "300.0", UIMax = "700.0"))
	float CohesionRadius = 500.f;

	// Separation Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Separation Parameters")
	float SeparationFactor = 0.4f;
	UPROPERTY(EditDefaultsOnly, Category = "Separation Parameters")
	float SeparationStressFactor = 0.05f;
	UPROPERTY(EditDefaultsOnly, Category = "Separation Parameters", meta = (ClampMin = "150.0", ClampMax = "500.0", UIMin = "150.0", UIMax = "500.0"))
	float SeparationRadius = 340.f;

	// Alignment Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters")
	float AlignmentFactor = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters")
	float AlignmentStressFactor = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters", meta = (ClampMin = "100.0", ClampMax = "400.0", UIMin = "100.0", UIMax = "400.0"))
	float AlignmentRadius = 300.f;

	// Escape Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Escape Parameters")
	float EscapeFactor = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Escape Parameters", meta = (ClampMin = "360.0", ClampMax = "480.0", UIMin = "360.0", UIMax = "480.0"))
	float EscapeRadius = 420.f;

	// Emotional Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Emotional Parameters")
	float EmotionalIncreaseFactor = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Emotional Parameters")
	float EmotionalDecreaseFactor = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Emotional Parameters", meta = (ClampMin = "0.5", ClampMax = "0.9", UIMin = "0.5", UIMax = "0.9"))
	float EmotionalStateMultiplier = 0.7f;
};