#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BoidData.generated.h"

UCLASS(BlueprintType)
class SHEEPESCAPE_API UBoidData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Stop Parameters")
	float MinVelocityToStopState = 0.2f;

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
	float SeparationStressFactor = 0.05f;
	UPROPERTY(EditDefaultsOnly, Category = "Separation Parameters", meta = (ClampMin = "150.0", ClampMax = "500.0", UIMin = "150.0", UIMax = "500.0"))
	float SeparationRadius = 340.f;

	// Alignment parameters
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters")
	float AlignmentFactor = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters")
	float AlignmentStressFactor = 0.15f;
	UPROPERTY(EditDefaultsOnly, Category = "Alignment Parameters", meta = (ClampMin = "100.0", ClampMax = "400.0", UIMin = "100.0", UIMax = "400.0"))
	float AlignmentRadius = 300.f;

	// Escape parameters
	UPROPERTY(EditDefaultsOnly, Category = "Escape Parameters")
	float EscapeFactor = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Escape Parameters", meta = (ClampMin = "360.0", ClampMax = "480.0", UIMin = "360.0", UIMax = "480.0"))
	float EscapeRadius = 420.f;

	// Emotional parameters
	UPROPERTY(EditDefaultsOnly, Category = "Emotional Parameters")
	float EmotionalIncreaseLerpFactor = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Emotional Parameters")
	float EmotionalDecreaseLerpFactor = 1.f;
};