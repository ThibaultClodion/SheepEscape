// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "NavigationSystem.h"
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
	void SetupVisualSphere();

	virtual void Tick(float DeltaTime) override;

	virtual void Eliminate() override;

protected:
	virtual void BeginPlay() override;

private:
	void Move(float DeltaTime);

	/** Attributes */
	float MaxSpeed = 500.f;
	float Acceleration = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Boid Behavior")
	float Inertia = 0.99f;
	UPROPERTY(EditDefaultsOnly, Category = "Boid Behavior")
	float MinVelocityLengthToMove = 0.12f;

	/** Boid Behavior */
	void BoidMovement();

	void UpdateBoidVelocity(float DeltaTime);
	FVector BoidVelocity;

	FVector Cohesion();
	FVector Separation();
	FVector Alignment();
	UPROPERTY(EditDefaultsOnly, Category = "Boid Behavior")
	float CohesionFactor = 0.04f;
	UPROPERTY(EditDefaultsOnly, Category = "Boid Behavior")
	float SeparationFactor = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Boid Behavior")
	float AlignmentFactor = 0.1f;

	/** Gazing Behavior */
	void GazingMovement();

	void StartGazing();
	void StopGazing();
	void InterruptGazing();
	bool IsGazing = false;

	UPROPERTY(EditDefaultsOnly, Category = "Gazing Behavior")
	float MinGazingTime = 3.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Gazing Behavior")
	float MaxGazingTime = 20.0f;
	FTimerHandle GazingTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Gazing Behavior")
	float MinVelocityLengthToStopGazing = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category = "Gazing Behavior")
	float GazingAlignmentFactor = 0.3f;

	/** Leading Behavior */
	void LeadMovement();

	void StartLeading(float LeadingTimeFactor);
	void StopLeading();
	void InterruptLeading();
	bool IsLeading = false;

	float MinLeadingTime = 1.f;
	float MaxLeadingTime = 5.0f;
	FTimerHandle LeadingTimer;

	FVector RandomLeadingInput = FVector::ZeroVector;

	/** Test : Random location on navmesh */
	void StartLeading2(float MinDistance, float MaxDistance);
	void StopLeading2();

	UPROPERTY(EditDefaultsOnly, Category = "Leading Behavior")
	float MinLittleLeadDistance = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Leading Behavior")
	float MaxLittleLeadDistance = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Leading Behavior")
	float MinBigLeadDistance = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Leading Behavior")
	float MaxBigLeadDistance = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Leading Behavior")
	float MinVelocityLengthToStopLead= 0.35f;

	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

	/** Boid Visual Sphere */
	UPROPERTY(VisibleAnywhere)
	USphereComponent* VisualSphere;

	void InitializeSphereOverlaps();

	UFUNCTION()
	virtual void OnVisualSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnVisualSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void AddingAlreadyOverlappingActors();
	void AddActorInVisualRange(AActor*& OtherActor);
	void RemoveActorInVisualRange(AActor*& OtherActor);

	/** Randomize Visual and Avoid Ranges */
	void RandomizeRange();
	FTimerHandle RandomizeRangeTimer;
	float RandomizeTime = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boid Behavior")
	float MaxVisualRange = 600.f;
	UPROPERTY(EditDefaultsOnly, Category = "Boid Behavior")
	float MinVisualRange = 400.f;
	float VisualRange;

	UPROPERTY(EditDefaultsOnly, Category = "Boid Behavior")
	float MaxAvoidRange = 265.f;
	UPROPERTY(EditDefaultsOnly, Category = "Boid Behavior")
	float MinAvoidRange = 235.f;
	float AvoidRange;

	/** Sheep in Visual Range and Shepherd */
	TArray<ABaseCharacter*> SheepInVisualRange;
	AShepherdCharacter* Shepherd;
};
