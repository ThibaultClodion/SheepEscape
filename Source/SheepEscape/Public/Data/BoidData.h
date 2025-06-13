#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BoidData.generated.h"

UCLASS(BlueprintType)
class SHEEPESCAPE_API USheepBotData : public UDataAsset
{
	GENERATED_BODY()

public:

	// Speed Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Speed Parameters")
	float Acceleration = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "Speed Parameters")
	float Inertia = 0.995f;

	// Stop Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Stop Parameters")
	float MinVelocityToStopBoidMove = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Stop Parameters")
	float MaxVelocityToStopGraze = 0.3f;

	// Cohesion Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Cohesion Parameters")
	float CohesionFactor = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Cohesion Parameters")
	float CohesionStressFactor = 0.5f;
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
	float AlignmentStressFactor = 0.15f;
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters", meta = (ClampMin = "100.0", ClampMax = "400.0", UIMin = "100.0", UIMax = "400.0"))
	float AlignmentRadius = 300.f;

	// Escape Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Escape Parameters")
	float EscapeFactor = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Escape Parameters", meta = (ClampMin = "360.0", ClampMax = "480.0", UIMin = "360.0", UIMax = "480.0"))
	float EscapeRadius = 420.f;

	// Emotional Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Emotional Parameters")
	float EmotionalIncreaseLerpFactor = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Emotional Parameters")
	float EmotionalDecreaseLerpFactor = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Emotional", meta = (ClampMin = "0.5", ClampMax = "0.9", UIMin = "0.5", UIMax = "0.9"))
	float EmotionalStateMultiplier = 0.7f;

	// Grazing - Wait Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MinWaitTime = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MaxWaitTime = 10.f;

	// Grazing - Walk Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MinWalkTime = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MaxWalkTime = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MinWalkVelocity = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MaxWalkVelocity = 0.7f;

	// Grazing - Run Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MinRunTime = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MaxRunTime = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MinRunVelocity = 0.7f;
	UPROPERTY(EditDefaultsOnly, Category = "Grazing Parameters")
	float MaxRunVelocity = 1.f;

	// Elimination Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Elimination Parameters")
	float MaxDistanceToEarnScore = 3000.f;
};